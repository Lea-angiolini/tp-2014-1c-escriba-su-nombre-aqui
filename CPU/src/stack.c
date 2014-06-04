#include "stack.h"

#include "commons/pcb.h"
#include "commons/log.h"
#include "commons/sockets.h"

#include <stdbool.h>

extern pcb_t PCB_enEjecucion;
extern t_log * logger;
extern int socketUMV;
Stack stackCache;


Stack new_Stack()
{
	Stack stack;
	stack.dataLength = 0;
	stack.offset = 0;
	return stack;
}


/* Apila una variable en el stack dado, y devuelve un "puntero" a esa variable */
//TODO, verificar si la variable no existe
uint32_t apilarVariable(char identificador)
{
	stackCache.data[stackCache.dataLength] = identificador;
	stackCache.dataLength += 5;
	stackCache.modificado = true;
	PCB_enEjecucion.contextSize += 5;
	PCB_enEjecucion.stackCursor += 5;
	return stackCache.dataLength - 4;
}


/*
 * Obtiene la posicion de memoria de una variable
 */

uint32_t obtenerOffsetVarible(char variable)
{
	int i = 0;
	for( i = 0; i < stackCache.dataLength ; i +=5 ) {
		if( stackCache.data[i] == variable ) {
			return i+1;
		}
	}
	log_error( logger, "Se solicito la posicion de memoria de una variable inexistente" );
	return 0;
}


//TODO ver segmentation fault
uint32_t obtenerValor(uint32_t pos)
{
	return (uint32_t)stackCache.data[pos];
}


//TODO verificar segmentation fault
void modificarVariable(uint32_t pos, uint32_t valor)
{
	stackCache.data[pos] = valor;
	stackCache.modificado = true;
}




/*
 * Se apila
 * direccion base del contecto
 * programCounter
 * posicion de la variable de retorno
 */
bool apilarFuncionConRetorno(uint32_t variableRetorno)
{
	log_warning(logger, "Funcion no implementada");
	apilarFuncionSinRetorno();
	stackCache.data[stackCache.dataLength] = variableRetorno;
	stackCache.dataLength += sizeof( uint32_t );
	PCB_enEjecucion.stackCursor += stackCache.dataLength;
	stackCache.modificado = true;
	return guardarStack();
}


bool apilarFuncionSinRetorno()
{
	printf("\n\nGuardando el contexto: %d - stackCursor, %d - contextSize, %d - ProgramCounter \n\n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);
	StackFuncion llamada;
	llamada.lastContextInit = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize;
	llamada.lastProgramCounter = PCB_enEjecucion.programCounter;
	memcpy(stackCache.data + stackCache.dataLength, &llamada, sizeof(StackFuncion)); //Copia la llamada al stackCache

	PCB_enEjecucion.stackCursor += sizeof(StackFuncion);
	PCB_enEjecucion.contextSize = sizeof(StackFuncion);
	stackCache.modificado = true;
	if(!guardarStack()){ //Guarda en la UMV solo la llamada, con length sizeof(StackFuncion) y offset en donde arranca la llamada
		log_error(logger, "Error al guardar la llamda a la funcion en el stack");
		return false;
	}

	PCB_enEjecucion.contextSize = 0;
	return true;
}



/*
 * Guarda en la umv el stack con el que estabamos trabajando
 */
//TODO manejar tamaño
bool guardarStack()
{

	if(PCB_enEjecucion.contextSize == 0){
		log_warning(logger, "Se envio a guardar el stack pero no se hizo porque el tamaño del contexto es 0");
		return true;
	}

	if(stackCache.modificado == false){
		log_warning(logger, "No se guardo el stack en la UMV porque no fue modificada");
		return true;
	}

	socket_guardarEnMemoria sGuardarEnMemoria;

	sGuardarEnMemoria.offset	= PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize;
	sGuardarEnMemoria.pdi		= PCB_enEjecucion.id;
	sGuardarEnMemoria.length	= PCB_enEjecucion.contextSize;
	sGuardarEnMemoria.base		= PCB_enEjecucion.stackSegment;
	memcpy(sGuardarEnMemoria.data, &stackCache.data, 100) ;

	socket_RespuestaGuardarEnMemoria * respuesta = (socket_RespuestaGuardarEnMemoria*) enviarYRecibirPaquete( socketUMV, &sGuardarEnMemoria, sizeof(socket_guardarEnMemoria) , 0, 'c', 'a', logger );
	if(respuesta == NULL || respuesta->status == false ) {
		log_error( logger, "Error al guardar el stack en la UMV" );
		return false;
	}else{
		log_debug( logger, "Stack guardado correctamente en la UMV" );
		return true;
	}

}



/*
 * Le solicita a la UMV los datos del stack desde el contexto actual
 * y los almacena en la "cache" del cpu.
 */
//TODO leer parcialmente y manejar error
bool obtenerContextStack()
{

	if( PCB_enEjecucion.contextSize == 0 ){
		return true;
	}

	socket_leerMemoria sLeerMemoria;

	sLeerMemoria.offset = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize;
	sLeerMemoria.pdi = PCB_enEjecucion.id;
	sLeerMemoria.base = PCB_enEjecucion.stackSegment;
	sLeerMemoria.length = PCB_enEjecucion.contextSize;

	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria*) enviarYRecibirPaquete(socketUMV, &sLeerMemoria, sizeof(socket_leerMemoria), 45, 'b', 'a', logger);
	if( respuesta == NULL || respuesta->status == false ) {
		log_error( logger, "Hubo un error al leer el stack" );
		return false;
	}else{
		memcpy( &stackCache.data, respuesta->data, PCB_enEjecucion.contextSize );
		stackCache.modificado = false;
		log_debug( logger, "Se leyo correctamente el stack desde la UMV" );
		return true;
	}

}



/*
 * Esto es para el caso que se llame al retorno de una funcion
 * Necesito conseguir todos los datos del contexto anterior, entonces
 * lo pido con esta funcion
 *
 */
bool obtenerContextStackAnterior(t_valor_variable retorno)
{

	printf("\nAhora el contexto es: %d - stackCursor, %d - contextSize, %d - ProgramCounter \n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);

	socket_leerMemoria sLeerStack;
	sLeerStack.pdi		= PCB_enEjecucion.id;
	sLeerStack.base		= PCB_enEjecucion.stackSegment;
	sLeerStack.length	= sizeof(StackFuncion);
	PCB_enEjecucion.stackCursor = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize - sizeof(StackFuncion) + 1;
	sLeerStack.offset	= PCB_enEjecucion.stackCursor;

	printf("Solicitando una lecutura con offset %d", sLeerStack.offset);
	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria *) enviarYRecibirPaquete(socketUMV, &sLeerStack, sizeof(socket_leerMemoria), sizeof(socket_RespuestaLeerMemoria) , 'b', 'a', logger) ;

	if( respuesta == NULL || respuesta->status == false ){
		log_error( logger, "Error al intentar retornar" );
		return false;
	}

	StackFuncion * respuestaStack	= (StackFuncion *) respuesta->data;

	PCB_enEjecucion.programCounter	= respuestaStack->lastProgramCounter;
	PCB_enEjecucion.contextSize		= PCB_enEjecucion.stackCursor - respuestaStack->lastContextInit;

	printf("Recibi la respuesat con lastProgramCounter: %d, lastContextInit: %d", respuestaStack->lastProgramCounter, respuestaStack->lastContextInit);
	printf("\nSe recupero el contexto: %d - stackCursor, %d - contextSize, %d - ProgramCounter \n\n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);
	if( !obtenerContextStack() ){
		log_error(logger,"Error al obtener el contexto");
		return false;
	}

	log_trace( logger, "Recuperado el contextStack anterior" );

	return true;
}

















