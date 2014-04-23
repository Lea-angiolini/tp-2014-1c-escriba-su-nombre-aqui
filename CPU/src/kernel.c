#include "commons/pcb.h"
#include "ejecucion.h"
#include "commons/sockets.h"


#define BUFF_SIZE 1024

extern int conexionKernel;
extern t_log * logger;
extern pcb_t * PCB_enEjecucion;




/*
 *	Instrucciones que el CPU puede recibir del kernel
 *
 *
 */


int orden_ejecucion( int socket, socket_header header ) {

	int  tamanio = header.size - sizeof(header);

	if( sizeof( pcb_t ) != tamanio ){
		log_error( logger, "El tamanio del paquete es incorrecto" );
		return -1;
	}

	int ret;
	ret = recv(socket, PCB_enEjecucion, tamanio, 0);
	if( ret <= 0 ){
		log_error( logger, "Error al leer el paquete del kernel" );
		return -1;
	}

	return responder_orden_ejecucion( ejecutar () );

}


//recibe un pcb despues de ser ejecutado y se lo re envia al kernel
int responder_orden_ejecucion( int socket, pcb_t * PCB ) {

	socket_header header;
	header.code = 's';
	header.size = sizeof(header)+sizeof( PCB );
	socket_msg msg;
	memcpy( &msg.msg, (void * ) PCB, sizeof( PCB ) );


	if ( send(socket, &msg, header.size, 0 ) > 0 ){
		free( PCB );
		return 1;
	}else{
		log_error( logger, "No pudo responderse el mensaje al Kernel" );
		free(PCB);
		return -1;
	}

}



/*
 *
 *
 */

//TODO definir codigos de operacion
int procesarMenssajeKernel( int socket, socket_header header ) {

	printf( "Me llego un mensaje del kernel" );

	int resultado;

	switch( header.code ){

		case 'a' :
			resultado = orden_ejecucion( socket, header );
			break;

	}

	return resultado;

}


//TODO se podria reusar lo de la umv ??
int recibirYProcesarMensajesKernel() {

	int nbytesRecibidos;
	socket_header header;

	while (1) {
		nbytesRecibidos = recv( conexionKernel, &header, BUFF_SIZE, 0 );
		if (nbytesRecibidos > 0) {
			if ( procesarMenssajeKernel( conexionKernel, header ) < 0 ) {
				break;
			}
		} else if ( nbytesRecibidos == 0 ) {
			log_info( logger, "Se desconecto el kernel" );
			return 1;
		} else {
			break;
		}
	}

	log_error( logger, "Hubo un error al leer el paquete enviado del kernel");
	return -1;
}

