#include <stdint.h>

#include "commons/log.h"
#include "commons/sockets.h"

extern int conexionUMV;
extern t_log * logger;





/*
 * Dado el program counter de un pcb, se le solicita a la UMV la linea de codigo a ejecutar
 *
 *
 */
char * solicitarLineaPrograma( uint32_t programCounter ) {

	log_info( logger, "Solicitando linea de programa");
	socket_obtenerLineaCodigo * paquete = malloc( sizeof ( socket_obtenerLineaCodigo ) ) ;
	paquete->numero_linea_Codigo = programCounter;

	//socket_responderLineaCodigo * paqueteRespuesta = (socket_responderLineaCodigo*)
	enviarYRecibirPaquete( conexionUMV, (void*) paquete, sizeof( socket_obtenerLineaCodigo ), sizeof( socket_responderLineaCodigo ) , 'a', 'd', logger  ) ;

	free( paquete );
	log_info( logger, "Se recibio una linea de programa");

	/*if ( paqueteRespuesta == NULL || paqueteRespuesta->numero_linea_Codigo != programCounter ){
		return -1;
	}*/

	//char respuesta[] = paqueteRespuesta->lineaCodigo ;
	//free(paqueteRespuesta);
	//return respuesta;
	static char array[] = "my string";
    return array;

}





/*
 * Informa a la UMV que este CPU ahora esta procesando el programa con PID pid
 * Siempre despues de este mensaje, cuando se termine el quantum se debe enviar el enviarFinQuantum
 *
 */
int enviarCambioContexto( uint32_t pid ) {

	//TODO
	log_debug( logger, "Enviando a umv cambio de contexto" );
	return 1;
}


/*
 * Envia que el programa finalizo para que destruya el segmento de memoria correspondiente
 *
 */
int enviarFinPrograma( uint32_t pid ){
	//TODO
	log_debug( logger, "Enviando a umv fin de programa" );
	return 1;
}




/*
 * Envia el mensaje que se termino el tiempo de ejecucion.
 * Si no se envia este mensaje y por alguna razon
 * El CPU se desconecta. La UMV eliminara el segmento de memoria.
 *
 */
int enviarFinQuantum( uint32_t pid ){
	//TODO
	log_debug( logger, "Enviando umv fin de quantum" );

	return 1;
}




/*
 * Guarda en la umv el stack con el que estabamos trabajando
 */
//TODO
int guardarStack(){

	socket_guardarEnMemoria * paquete = malloc( sizeof( socket_guardarEnMemoria ) );

	paquete->offset = 0;
	paquete->pdi = 1;
	paquete->length = 10;
	char data[100] = "probando ";
	memcpy( paquete->data, data, 10 ) ;

	socket_RespuestaGuardarEnMemoria * respuesta = (socket_RespuestaGuardarEnMemoria*) enviarYRecibirPaquete( conexionUMV, paquete, sizeof(socket_guardarEnMemoria) , 45, 'c', 'a', logger );
	if( respuesta == NULL || respuesta->status ) {
		printf( "La umv me cago\n");
		return -1;
	}else{
		printf( "La umv respondio\n" );
		return 1;
	}


}



/*
 * Le solicita a la UMV los datos del stack desde el contexto actual
 * y los almacena en la "cache" del cpu.
 */
//TODO
int obtenerContextStack() {

	socket_leerMemoria * paquete = malloc(sizeof( socket_leerMemoria ) );

	paquete->offset = 2;
	paquete->pdi = 1;
	paquete->length = 4;

	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria*) enviarYRecibirPaquete( conexionUMV, paquete, sizeof(socket_leerMemoria), 45, 'b', 'a', logger );
	if( respuesta == NULL || respuesta->status == false ) {
		printf( "La umv me cago\n");
		return -1;
	}else{
		printf( "La umv respondio: %s\n", respuesta->data );
		return 1;
	}

}



/*
 * Esto es para el caso que se llame al retorno de una funcion
 * Necesito conseguir todos los datos del contexto anterior, entonces
 * lo pido con esta funcion
 *
 */
int obtenerContextStackAnterior(){
	//Tiene que hacer 2 llamadas a la umv
	return 1;
}




















































