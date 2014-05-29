#include "umv.h"
#include "config.h"

#include "commons/log.h"
#include "commons/sockets.h"
#include "commons/parser/parser.h"

#include <unistd.h>

extern t_log * logger;
extern pcb_t PCB_enEjecucion;
int socketUMV;


bool crearConexionUMV() {
	log_info(logger, "Conectando a la UMV en %s:%d", config_get_string_value(config, "IPUMV"), config_get_int_value(config, "PUERTOUMV"));
	socketUMV = conectar(config_get_string_value(config, "IPUMV"),	config_get_int_value(config, "PUERTOUMV"), logger);

	if (socketUMV < 0) {
		log_error(logger, "No se pudo conectar al UMV");
		close( socketUMV );
		return false;
	}
	return true;
}


/*
 * Dado el program counter de un pcb, se le solicita a la UMV la linea de codigo a ejecutar
 *
 *
 */

char * solicitarLineaPrograma() {


	/*switch (programCounter){
	case 0:
		return "variables a";
		break;
	case 1:
		return "a=1";
		break;
	case 2:
		return "print a";
		break;
	case 3:
		return "variables b";
		break;
	case 4:
		return "a=2";
		break;
	case 5:
		return "a=8";
		break;
	case 6:
		return "a=90";
		break;
	case 7:
		return "b=2";
		break;
	case 8:
		return "print b";
		break;
	default:
		return "end";
	}*/

	log_info( logger, "Solicitando linea de programa a la UMV");
	socket_leerMemoria sLeerMemoria;
	log_debug( logger, "Solicitando segmento con base %d", PCB_enEjecucion.codeSegment);
	sLeerMemoria.base = PCB_enEjecucion.codeSegment;
	sLeerMemoria.offset = PCB_enEjecucion.programCounter;

	socket_RespuestaLeerMemoria * paqueteRespuesta = (socket_RespuestaLeerMemoria *) enviarYRecibirPaquete( socketUMV, (void*)&sLeerMemoria, sizeof( socket_leerMemoria ), sizeof( socket_RespuestaLeerMemoria ) , 'b', 'a', logger  ) ;

	if ( paqueteRespuesta == NULL ) {
		return -1;
	}

	char * respuesta = paqueteRespuesta->data;
	printf("Se obtuvo la linea %s\n", respuesta );
	free(paqueteRespuesta);
	return respuesta;

}


uint32_t obtenerLineaDeLabel( t_nombre_etiqueta t_nombre_etiqueta ) {
	return 1;
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




