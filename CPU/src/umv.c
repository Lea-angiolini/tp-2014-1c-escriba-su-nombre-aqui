#include <stdint.h>



char * solicitarLineaPrograma(){
	return "a+b";
}



/*
 * Informa a la UMV que este CPU ahora esta procesando el programa con PID pid
 * Siempre despues de este mensaje, cuando se termine el quantum se debe enviar el enviarFinQuantum
 *
 */
int enviarCambioContexto( uint32_t pid ){
	return 1;
}


/*
 * Envia que el programa finalizo para que destruya el segmento de memoria correspondiente
 *
 */
int enviarFinPrograma( uint32_t pid ){

	return 1;
}




/*
 * Envia el mensaje que se termino el tiempo de ejecucion.
 * Si no se envia este mensaje y por alguna razon
 * El CPU se desconecta. La UMV eliminara el segmento de memoria.
 *
 */
int enviarFinQuantum( uint32_t pid ){
	return 1;
}




















