#include "commons/pcb.h"
#include "ejecucion.h"

#define BUFF_SIZE 1024

extern int conexionKernel;






/*
 *	Instrucciones que el CPU puede recibir del kernel
 *
 *
 */


int orden_ejecucion(){

	pcb_t PCB;
	/* .......  */


	return responder_orden_ejecucion( ejecutar ( PCB ) );

}


//recibe un pcb despues de ser ejecutado y se lo re envia al kernel
int responder_orden_ejecucion( pcb_t PCB ){
	return 1;
}



/*
 *
 *
 */



int procesarMenssajeKernel( char * mensaje ){

	printf( "Me llego un mensaje del kernel: %s \n", mensaje );
	return 1;

}


//TODO se podria reusar lo de la umv ??
int recibirYProcesarMensajesKernel() {

	int nbytesRecibidos;
	char * buffer = malloc( sizeof( BUFF_SIZE ) );

	while (1) {

		nbytesRecibidos = recv( conexionKernel, buffer, BUFF_SIZE, 0 );

		if (nbytesRecibidos > 0) {

			procesarMenssajeKernel(  buffer );
			memset( buffer, 0x0000, BUFF_SIZE );

		} else if ( nbytesRecibidos == 0 ) {

			free( buffer );
			return 1;

		} else {

			break;

		}

	}

	free(buffer);

	return -1;

}

