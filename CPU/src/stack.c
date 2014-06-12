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
	return stack;
}


/* Apila una variable en el stack dado, y devuelve un "puntero" a esa variable */
//TODO, verificar si la variable no existe
uint32_t apilarVariable(char identificador)
{
	stackCache.data[PCB_enEjecucion.stackCursor] = identificador;
	stackCache.modificado = true;
	PCB_enEjecucion.contextSize += 5;
	PCB_enEjecucion.stackCursor += 5;
	return PCB_enEjecucion.stackCursor - 4;
}


/*
 * Obtiene la posicion de memoria de una variable
 */

uint32_t obtenerOffsetVarible(char variable)
{
	int i = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize;
	log_debug(logger, "Analizando el stack desde: %d hasta: %d", i, PCB_enEjecucion.stackCursor);
	for(; i < PCB_enEjecucion.stackCursor; i +=5 ) {
		if( stackCache.data[i] == variable ) {
			return i+1;
		}
	}
	//PCB_enEjecucion.lastErrorCode = 3;
	log_error( logger, "Se solicito la posicion de memoria de una variable inexistente: %c", variable );
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
	guardarStack();
	log_debug(logger, "\nApliando returno: %d - %c\n", variableRetorno, variableRetorno);
	//printf("\nGuardando el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d \n\n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);
	StackFuncionConRetorno llamada;
	llamada.lastContextInit = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize;
	llamada.lastProgramCounter = PCB_enEjecucion.programCounter;
	llamada.variableRetorno = variableRetorno;
	memcpy(stackCache.data + PCB_enEjecucion.stackCursor, &llamada, sizeof(StackFuncionConRetorno)); //Copia la llamada al stackCache

	socket_guardarEnMemoria sGuardarEnMemoria;
	sGuardarEnMemoria.offset	= PCB_enEjecucion.stackCursor;
	sGuardarEnMemoria.pdi		= PCB_enEjecucion.id;
	sGuardarEnMemoria.length	= sizeof(StackFuncionConRetorno);
	sGuardarEnMemoria.base		= PCB_enEjecucion.stackSegment;
	memcpy(sGuardarEnMemoria.data, &llamada, sizeof(StackFuncionConRetorno));

	PCB_enEjecucion.stackCursor += sizeof(StackFuncionConRetorno);
	PCB_enEjecucion.contextSize = 0;
	stackCache.modificado = false;

	socket_RespuestaGuardarEnMemoria * respuesta = (socket_RespuestaGuardarEnMemoria*) enviarYRecibirPaquete( socketUMV, &sGuardarEnMemoria, sizeof(socket_guardarEnMemoria) , 0, 'c', 'a', logger );
	if(respuesta == NULL || respuesta->status == false ) {
		log_error( logger, "Error al apilar la funcion en la UMV" );
		return false;
	}else{
		log_debug( logger, "Funcion guardada correctamente en la UMV" );
		return true;
	}
	return true;
}



bool apilarFuncionSinRetorno()
{
	guardarStack();
	printf("\nGuardando el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d \n\n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);
	StackFuncion llamada;
	llamada.lastContextInit = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize;
	llamada.lastProgramCounter = PCB_enEjecucion.programCounter;
	memcpy(stackCache.data + PCB_enEjecucion.stackCursor, &llamada, sizeof(StackFuncion)); //Copia la llamada al stackCache

	socket_guardarEnMemoria sGuardarEnMemoria;
	sGuardarEnMemoria.offset	= PCB_enEjecucion.stackCursor;
	sGuardarEnMemoria.pdi		= PCB_enEjecucion.id;
	sGuardarEnMemoria.length	= sizeof(StackFuncion);
	sGuardarEnMemoria.base		= PCB_enEjecucion.stackSegment;
	memcpy(sGuardarEnMemoria.data, &llamada, sizeof(StackFuncion));

	PCB_enEjecucion.stackCursor += sizeof(StackFuncion);
	PCB_enEjecucion.contextSize = 0;
	stackCache.modificado = false;

	socket_RespuestaGuardarEnMemoria * respuesta = (socket_RespuestaGuardarEnMemoria*) enviarYRecibirPaquete( socketUMV, &sGuardarEnMemoria, sizeof(socket_guardarEnMemoria) , 0, 'c', 'a', logger );
	if(respuesta == NULL || respuesta->status == false ) {
		log_error( logger, "Error al apilar la funcion en la UMV" );
		return false;
	}else{
		log_debug( logger, "Funcion guardada correctamente en la UMV" );
		return true;
	}
	return true;

}



/*
 * Guarda en la umv el stack con el que estabamos trabajando
 */
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
	memcpy(sGuardarEnMemoria.data, stackCache.data + sGuardarEnMemoria.offset, sGuardarEnMemoria.length) ;

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
bool retornarVoid()
{

	printf("\nAhora el contexto es: stackCursor = %d, contextSize = %d, ProgramCounter = %d \n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);

	socket_leerMemoria sLeerStack;
	sLeerStack.pdi		= PCB_enEjecucion.id;
	sLeerStack.base		= PCB_enEjecucion.stackSegment;
	sLeerStack.length	= sizeof(StackFuncion);
	PCB_enEjecucion.stackCursor = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize - sizeof(StackFuncion);
	sLeerStack.offset	= PCB_enEjecucion.stackCursor;


	printf("Solicitando una lecutura para recuperar el stack con offset = %d y length = %d\n", sLeerStack.offset, sLeerStack.length);
	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria *) enviarYRecibirPaquete(socketUMV, &sLeerStack, sizeof(socket_leerMemoria), sizeof(socket_RespuestaLeerMemoria) , 'b', 'a', logger) ;

	if( respuesta == NULL || respuesta->status == false ){
		log_error( logger, "Error al intentar retornar" );
		return false;
	}

	StackFuncion * respuestaStack	= (StackFuncion *) respuesta->data;
	PCB_enEjecucion.programCounter	= respuestaStack->lastProgramCounter;
	PCB_enEjecucion.contextSize		= PCB_enEjecucion.stackCursor - respuestaStack->lastContextInit;

	printf("Recibi la respuesta con lastProgramCounter: %d, lastContextInit: %d", respuestaStack->lastProgramCounter, respuestaStack->lastContextInit);
	printf("\nSe recupero el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d \n\n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);
	free(respuesta);
	if( !obtenerContextStack() ){
		log_error(logger,"Error al obtener el contexto");
		return false;
	}

	log_trace( logger, "Recuperado el contextStack anterior" );

	return true;
}



bool retornarValor(t_valor_variable retorno){


	log_debug(logger, "Se debe devolver el valor %d", retorno);

	//printf("\nAhora el contexto es: stackCursor = %d, contextSize = %d, ProgramCounter = %d \n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);

	socket_leerMemoria sLeerStack;
	sLeerStack.pdi		= PCB_enEjecucion.id;
	sLeerStack.base		= PCB_enEjecucion.stackSegment;
	sLeerStack.length	= sizeof(StackFuncionConRetorno);
	PCB_enEjecucion.stackCursor = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize - sizeof(StackFuncionConRetorno);
	sLeerStack.offset	= PCB_enEjecucion.stackCursor;


	//printf("Solicitando una lecutura para recuperar el stack con offset = %d y length = %d\n", sLeerStack.offset, sLeerStack.length);
	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria *) enviarYRecibirPaquete(socketUMV, &sLeerStack, sizeof(socket_leerMemoria), sizeof(socket_RespuestaLeerMemoria) , 'b', 'a', logger) ;

	if( respuesta == NULL || respuesta->status == false ){
		log_error( logger, "Error al intentar retornar" );
		return false;
	}

	StackFuncionConRetorno * respuestaStack	= (StackFuncionConRetorno *) respuesta->data;
	PCB_enEjecucion.programCounter	= respuestaStack->lastProgramCounter;
	PCB_enEjecucion.contextSize		= PCB_enEjecucion.stackCursor - respuestaStack->lastContextInit;


	//printf("Recibi la respuesta con lastProgramCounter: %d, lastContextInit: %d", respuestaStack->lastProgramCounter, respuestaStack->lastContextInit);
	//printf("\nSe recupero el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d \n\n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);

	if( !obtenerContextStack() ){
		log_error(logger,"Error al obtener el contexto");
		free(respuesta);
		return false;
	}


	log_debug(logger, "Guardando el valor en la variable de retorno, vale: %d, la guardo en la pos: %d", retorno, respuestaStack->variableRetorno);
	modificarVariable(respuestaStack->variableRetorno, retorno);

	log_trace( logger, "Recuperado el contextStack anterior" );
	free(respuesta);

	return true;
}









