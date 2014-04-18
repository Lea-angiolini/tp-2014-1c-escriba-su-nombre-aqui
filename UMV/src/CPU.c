#include "commons/collections/list.h"
#include "commons/log.h"
#include "CPU.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

extern t_log * logger;

//TODO liberar esta lista al finalizar el programa
extern t_list * cpus;


int contadorCpuId;


int procesarMensajeCpu( CPU * cpu, const char * mensaje ){

	printf( "Me llego un mensaje a procesar: %s \n", mensaje );
	return 1;

}


int recibirYProcesarMensajesCpu( CPU * cpu ){

	int nbytesRecibidos;
	char * buffer = malloc(sizeof( BUFF_SIZE ));

	while( 1 ){

		nbytesRecibidos = recv( cpu->socket, buffer, BUFF_SIZE, 0);

		if ( nbytesRecibidos > 0 ) {

			procesarMensajeCpu( cpu, buffer );
			memset( buffer, 0x0000, BUFF_SIZE );

		} else if ( nbytesRecibidos == 0 ){

			free(buffer);
			return 1;

		} else {

			break;

		}

	}

	free( buffer );

	return -1;

}





void * fnNuevoCpu( void * socketPtr ){

	log_info( logger, "Se conecto un nuevo CPU" );


	//TODO al desconectar sacar el cpu de la lista de CPUs
	contadorCpuId++;
	CPU * cpu = malloc(sizeof(CPU));
	cpu->socket = *(( int * ) socketPtr);
	cpu->cpuId = contadorCpuId;
	free( socketPtr );
	list_add( cpus, cpu );


	if( recibirYProcesarMensajesCpu( cpu ) > 0 ){
		log_info( logger, "Se desconecto un cpu correctamente" );
	}else{
		log_error( logger, "Hubo un problema con el cpu ID %d: " , cpu->cpuId );
	}

	close( cpu ->socket );

	return 0x0000;

}













