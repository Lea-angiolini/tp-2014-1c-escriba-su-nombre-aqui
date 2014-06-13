#include "umv.h"
#include "config.h"

#include "commons/log.h"
#include "commons/sockets.h"
#include "commons/parser/parser.h"
#include "commons/parser/metadata_program.h"
#include "commons/string.h"

#include <unistd.h>

extern t_log * logger;
extern pcb_t PCB_enEjecucion;
int socketUMV;
char * etiquetasCache;

bool crearConexionUMV() {

	log_debug(logger, "Conectando a la UMV en %s:%d", config_get_string_value(config, "IPUMV"), config_get_int_value(config, "PUERTOUMV"));
	socketUMV = conectar(config_get_string_value(config, "IPUMV"),	config_get_int_value(config, "PUERTOUMV"), logger);

	if (socketUMV < 0) {
		log_error(logger, "No se pudo conectar al UMV");
		close( socketUMV );
		return false;
	}

	socket_header handshake;
	handshake.code = 'c';
	handshake.size = sizeof(socket_header);
	if(send(socketUMV, &handshake, handshake.size, 0 ) > 0 ) {
		return true;
	}
	log_error(logger, "Error al enviar el handshake a la UMV");
	return false;
}


/*
 * Dado el program counter de un pcb, se le solicita a la UMV la linea de codigo a ejecutar
 *
 *
 */

char * solicitarLineaPrograma() {

	log_debug( logger, "Solicitando linea de programa a la UMV para el programCounter: %d", PCB_enEjecucion.programCounter );

	socket_leerMemoria sLeerCodeIndex;
	sLeerCodeIndex.pdi = PCB_enEjecucion.id;
	sLeerCodeIndex.base = PCB_enEjecucion.codeIndex;
	sLeerCodeIndex.offset = sizeof(t_intructions) * PCB_enEjecucion.programCounter;
	sLeerCodeIndex.length = sizeof(t_intructions);
	socket_RespuestaLeerMemoria * respuestaCodeIndex = (socket_RespuestaLeerMemoria *) enviarYRecibirPaquete( socketUMV, (void*)&sLeerCodeIndex, sizeof( socket_leerMemoria ), sizeof( socket_RespuestaLeerMemoria ) , 'b', 'a', logger  ) ;
	if ( respuestaCodeIndex == NULL || respuestaCodeIndex->status == false ) {
		log_error( logger, "La UMV respondio con un error o Segmentation fault" );
		return (char *)-1;
	}

	t_intructions * instruct = (t_intructions *) respuestaCodeIndex->data ;
	log_debug( logger, "Se leyo el codeIndex, la proxima instruccion esta en %d - %d", instruct->offset, instruct->start );

	if(instruct->offset == 0){
		log_error( logger, "Llego un code index con length 0 | ( %s ) %s - %s",  __func__, __FILE__, __LINE__  );
		return (char *) -1;
	}else if( instruct->offset <= 3 ){
		log_warning( logger, "Llego un code index con length de 3 o menos" );
	}

	socket_leerMemoria sLeerLineaCodigo;
	sLeerLineaCodigo.pdi = PCB_enEjecucion.id;
	sLeerLineaCodigo.base = PCB_enEjecucion.codeSegment;
	sLeerLineaCodigo.offset = instruct->start;
	sLeerLineaCodigo.length = instruct->offset;

	socket_RespuestaLeerMemoria * paqueteRespuesta = (socket_RespuestaLeerMemoria *) enviarYRecibirPaquete( socketUMV, (void*)&sLeerLineaCodigo, sizeof( socket_leerMemoria ), sizeof( socket_RespuestaLeerMemoria ) , 'b', 'a', logger  ) ;

	if ( paqueteRespuesta == NULL || paqueteRespuesta->data == NULL || paqueteRespuesta->status == false ) {
		return (char *)-1;
	}

	char * respuesta = malloc(instruct->offset);
	memcpy(respuesta, paqueteRespuesta->data, instruct->offset);
	free(paqueteRespuesta);
	eliminarSaltoLinea(respuesta);
	return respuesta;

}

void eliminarSaltoLinea(char * linea){
	int i = 0;
	for(i = 0; i < strlen(linea); i++){
		if (linea[i] == '\n'){
			linea[i] = '\0';
		}
	}
}



bool obtenerEtiquetas(){

	if( PCB_enEjecucion.etiquetasSize == 0 ){
		etiquetasCache = realloc( etiquetasCache, 1 );
		log_warning( logger, "No leyeron etiquetas, el programa tiene?" );
		return true;
	}

	socket_leerMemoria sLeerCodeIndex;
	sLeerCodeIndex.pdi = PCB_enEjecucion.id;
	sLeerCodeIndex.base = PCB_enEjecucion.etiquetaIndex;
	sLeerCodeIndex.offset = 0;
	sLeerCodeIndex.length = PCB_enEjecucion.etiquetasSize;
	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria *) enviarYRecibirPaquete( socketUMV, (void*)&sLeerCodeIndex, sizeof( socket_leerMemoria ), sizeof( socket_RespuestaLeerMemoria ) , 'b', 'a', logger  ) ;

	if( respuesta == NULL || respuesta->status == false ) {
		log_error( logger, "Llego un code index con length 0 | ( %s ) %s - %s  ",  __func__, __FILE__, __LINE__ );
		return false;
	}

	etiquetasCache = realloc( etiquetasCache, PCB_enEjecucion.etiquetasSize );
	memcpy( etiquetasCache, respuesta->data, PCB_enEjecucion.etiquetasSize );
	free( respuesta );
	return true;

}


uint32_t obtenerLineaDeLabel( t_nombre_etiqueta t_nombre_etiqueta ) {
	//t_nombre_etiqueta[ strlen(t_nombre_etiqueta) -1 ] = '\0';
	int i=0;
	int offset = 0;
	char* nombre;
	for(i=0;  offset < PCB_enEjecucion.etiquetasSize; i++){
	nombre = etiquetasCache + offset;
		if( string_equals_ignore_case(nombre, t_nombre_etiqueta) )
			return *(nombre + 1 + strlen(nombre));
		offset += strlen(nombre) + 1 + sizeof(t_puntero_instruccion);
	}
	return -1;
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




