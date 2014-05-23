#include "umv.h"
#include "config.h"

#include "commons/log.h"
#include "commons/sockets.h"


extern t_log * logger;

int socketUMV;


bool crearConexionUMV() {
	log_info(logger, "Conectando a la UMV en %s:%d", config_get_string_value(config, "IPUMV"), config_get_int_value(config, "PUERTOUMV"));
	socketUMV = conectar(config_get_string_value(config, "IPUMV"),	config_get_int_value(config, "PUERTOUMV"), logger);

	if (socketUMV < 0) {
		log_error(logger, "No se pudo conectar al UMV");
		return false;
	}
	return true;
}


/*
 * Dado el program counter de un pcb, se le solicita a la UMV la linea de codigo a ejecutar
 *
 *
 */
char * solicitarLineaPrograma( uint32_t programCounter ) {

	log_info( logger, "Solicitando linea de programa");
	socket_obtenerLineaCodigo sObtenerLineaCodigo;

	sObtenerLineaCodigo.numero_linea_Codigo = programCounter;

	//socket_responderLineaCodigo * paqueteRespuesta = (socket_responderLineaCodigo*)
	enviarYRecibirPaquete( socketUMV, &sObtenerLineaCodigo, sizeof( socket_obtenerLineaCodigo ), sizeof( socket_responderLineaCodigo ) , 'a', 'd', logger  ) ;

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
int guardarStack()
{

	socket_guardarEnMemoria sGuardarEnMemoria;

	sGuardarEnMemoria.offset = 0;
	sGuardarEnMemoria.pdi = 1;
	sGuardarEnMemoria.length = 100;
	memcpy( sGuardarEnMemoria.data, stackCache->data, 100 ) ;

	socket_RespuestaGuardarEnMemoria * respuesta = (socket_RespuestaGuardarEnMemoria*) enviarYRecibirPaquete( socketUMV, &sGuardarEnMemoria, sizeof(socket_guardarEnMemoria) , 0, 'c', 'a', logger );
	if( respuesta == NULL || respuesta->status ) {
		return -1;
	}else{
		return 1;
	}


}



/*
 * Le solicita a la UMV los datos del stack desde el contexto actual
 * y los almacena en la "cache" del cpu.
 */
//TODO leer parcialmente y manejar error
int obtenerContextStack()
{

	socket_leerMemoria sLeerMemoria;

	sLeerMemoria.offset = 0;
	sLeerMemoria.pdi = 1;
	sLeerMemoria.length = 100;

	socket_RespuestaLeerMemoria * respuesta = (socket_RespuestaLeerMemoria*) enviarYRecibirPaquete( socketUMV, &sLeerMemoria, sizeof(socket_leerMemoria), 45, 'b', 'a', logger );
	if( respuesta == NULL || respuesta->status == false ) {
		log_error( logger, "Hubo un error al leer el stack" );
		return -1;
	}else{
		memcpy( stackCache->data, respuesta->data, respuesta->header.size );
		log_debug( logger, "Se leyo correctamente el stack desde la UMV" );
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

