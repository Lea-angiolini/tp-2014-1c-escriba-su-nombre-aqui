#include <stdint.h>
#include "commons/log.h"


extern t_log * logger;

/*
 * Dado el program counter de un pcb, se le solicita a la UMV la linea de codigo a ejecutar
 *
 *
 */
char * solicitarLineaPrograma( uint32_t programCounter ){
	//TODO
	char month[]="a = 3";

	return month;
}



/*
 * Informa a la UMV que este CPU ahora esta procesando el programa con PID pid
 * Siempre despues de este mensaje, cuando se termine el quantum se debe enviar el enviarFinQuantum
 *
 */
int enviarCambioContexto( uint32_t pid ){
	//TODO
	log_debug( logger, "Enviando a umv cambio de contexto" );
	return 1;
}


/*
 * Envia que el programa finalizo para que destruya el segmento de memoria correspondiente
 *
 */
int enviarFinPrograma( uint32_t pid ){
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




















