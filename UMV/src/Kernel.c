#include "commons/log.h"
#include "Kernel.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024


extern t_log * logger;



int procesarMenssajeKernel( Kernel * kernel, char * mensaje ){

	printf( "Me llego un mensaje del kernel: %s \n", mensaje );
	return 1;

}





//TODO se podria reusar lo de cpu ??
int recibirYProcesarMensajesKernel(Kernel * kernel) {

	int nbytesRecibidos;
	char * buffer = malloc( sizeof( BUFF_SIZE ) );

	while (1) {

		nbytesRecibidos = recv(kernel->socket, buffer, BUFF_SIZE, 0);

		if (nbytesRecibidos > 0) {

			procesarMenssajeKernel( Kernel, buffer );
			memset(buffer, 0x0000, BUFF_SIZE);

		} else if (nbytesRecibidos == 0) {

			free(buffer);
			return 1;

		} else {

			break;

		}

	}

	free(buffer);

	return -1;

}




void * fnKernelConectado( void * socketPtr ){


	log_info( logger, "Se conecto el kernel" );

	Kernel * kernel = malloc( sizeof( Kernel ) );
	kernel->socket = *((int*)socketPtr);
	free(socketPtr);


	if( recibirYProcesarMensajesKernel( kernel ) > 0 ){
		log_info( logger, "El kernel se desconecto correctamente" );
	}else{
		log_error( logger, "Hubo un problema con el kernel" );
	}

	//TODO hacer que cuando se desconecte el kernel finalice la UMV

	free( kernel );

	return 0x0000;

}





