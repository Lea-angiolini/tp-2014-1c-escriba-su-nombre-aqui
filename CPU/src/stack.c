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


/*
 *	Apila una variable en el stack dado, y devuelve un "puntero" a esa variable
 */
//TODO, verificar si la variable no existe
uint32_t apilarVariable( char identificador )
{
	stackCache.data[stackCache.dataLength] = identificador;
	stackCache.dataLength += 5;
	return stackCache.dataLength - 4;
}


/*
 * Obtiene la posicion de memoria de una variable
 */

uint32_t obtenerOffsetVarible( char variable )
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
uint32_t obtenerValor( uint32_t pos )
{
	return (uint32_t)stackCache.data[pos];
}


//TODO verificar segmentation fault
void modificarVariable( uint32_t pos, uint32_t valor )
{
	stackCache.data[pos] = valor;
}




/*
 * Se apila
 * direccion base del contecto
 * programCounter
 * posicion de la variable de retorno
 */
bool apilarFuncionConRetorno( uint32_t variableRetorno )
{
	apilarFuncionSinRetorno();
	stackCache.data[stackCache.dataLength] = variableRetorno;
	stackCache.dataLength += sizeof( uint32_t );
	return true;
}


bool apilarFuncionSinRetorno()
{
	stackCache.data[stackCache.dataLength] = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize;
	stackCache.data[stackCache.dataLength +  sizeof( uint32_t ) ] = PCB_enEjecucion.programCounter;
	stackCache.dataLength += 2*sizeof( uint32_t );
	return true;
}



/*
 * Guarda en la umv el stack con el que estabamos trabajando
 */
//TODO manejar tamaño
bool guardarStack()
{

	socket_guardarEnMemoria sGuardarEnMemoria;

	sGuardarEnMemoria.offset = 0;
	sGuardarEnMemoria.pdi = PCB_enEjecucion.id;
	sGuardarEnMemoria.length = PCB_enEjecucion.contextSize;
	sGuardarEnMemoria.base = 0;
	memcpy( sGuardarEnMemoria.data, &stackCache.data, 100 ) ;

	/*printf("\n\n\n");
	int i = 0;
	for( i = 0; i < 24; i+=5){
		printf("%c = %d\n", sGuardarEnMemoria.data[ i ], sGuardarEnMemoria.data[ i+1 ] );
	}
	printf("\n\n\n");*/

	socket_RespuestaGuardarEnMemoria * respuesta = (socket_RespuestaGuardarEnMemoria*) enviarYRecibirPaquete( socketUMV, &sGuardarEnMemoria, sizeof(socket_guardarEnMemoria) , 0, 'c', 'a', logger );
	if( respuesta == NULL || respuesta->status ) {
		return false;
	}else{
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
	sLeerMemoria.base = 0;
	sLeerMemoria.length = PCB_enEjecucion.contextSize;

	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria*) enviarYRecibirPaquete( socketUMV, &sLeerMemoria, sizeof(socket_leerMemoria), 45, 'b', 'a', logger );
	if( respuesta == NULL || respuesta->status == false ) {
		log_error( logger, "Hubo un error al leer el stack" );
		return false;
	}else{
		memcpy( &stackCache.data, respuesta->data, respuesta->header.size );
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
bool obtenerContextStackAnterior(){
	//Tiene que hacer 2 llamadas a la umv
	return 1;
}

















