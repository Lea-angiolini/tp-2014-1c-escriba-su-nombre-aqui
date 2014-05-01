#include "Kernel.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "memoria.h"

#include "commons/log.h"
#include "commons/sockets.h"

#define BUFF_SIZE 1024


extern t_log * logger;



//TODO se podria reusar lo de cpu ??
int recibirYProcesarMensajesKernel(Kernel * kernel) {
	int nbytesRecibidos;
	socket_pedirMemoria buffer = malloc( sizeof( socket_pedirMemoria ) );

	while (1) {

		nbytesRecibidos = recv(kernel->socket, &buffer, BUFF_SIZE, 0);

		if (nbytesRecibidos > 0) {

			procesarMenssajeKernel( kernel, buffer );
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



int procesarMenssajeKernel( Kernel * kernel, socket_pedirMemoria * segmentosAreservar ){

		bool respuesta = true;
		int memoriaDisponible;
		int tamanioTotalSegmentos = tamanioSegmentos( segmentosAreservar );
		memoriaDisponible = memoriaLibre();

		if( tamanioTotalSegmentos > memoriaDisponible )
			respuesta = false;

		socket_respuesta respuestaSegmentos;
		respuestaSegmentos.header.size = sizeof( respuestaSegmentos );
		respuestaSegmentos.valor = respuesta;

		if(send(kernel->socket, &respuestaSegmentos, sizeof(respuestaSegmentos), 0) == -1){
			log_error(logger, "No se ha podido enviar respuesta de reservación de segmentos al Kernel");
		}

		if( respuesta == true ){

			socket_umvpcb datosSegmentosCreados;
			char * script, etiquetas, instrucciones;
			int tamanioScript, tamanioEtiquetas, tamanioInstrucciones;
			int tamanioStack = config_get_int_value(umvConfig, "TAMANIOSTACK");

			tamanioScript = rcv( kernel->socket, &script, BUFF_SIZE, 0 );
			tamanioEtiquetas = rcv( kernel->socket, &etiquetas, BUFF_SIZE, 0);
			tamanioInstrucciones = rcv( kernel->socket, &instrucciones, BUFF_SIZE, 0);

			if( tamanioScript < 0 || tamanioEtiquetas < 0 || tamanioInstrucciones < 0){
				datosSegmentosCreados->stackSegment = crearSegmento( tamanioStack);
				datosSegmentosCreados->codeSegment = crearYllenarSegmento( tamanioScript, &script );
				datosSegmentosCreados->etiquetaIndex = crearYllenarSegmento( tamanioEtiquetas, &etiquetas );
				datosSegmentosCreados->codeIndex = crearYllenarSegmento( tamanioInstrucciones, &instrucciones );

				if(send( kernel->socket, &datosSegmentosCreados, sizeof(datosSegmentosCreados), 0) < 0){
							log_error( logger, "No se ha podido enviar los datos de los segmentos creados correctamente al Kernel");
						}
					else{
						log_info(logger, "Se ha enviado los datos de los segmentos creados correctamente al Kernel");
						return 1;
						}
					}
				}
		return 1;
	}


int tamanioSegmentos( socket_pedirMemoria * segmentosAreservar){
	int tamanioStack = config_get_int_value(umvConfig, "TAMANIOSTACK");
	return(segmentosAreservar->codeSegmentSize + segmentosAreservar->etiquetasSegmentSize + segmentosAreservar->instruccionesSegmentSize, tamanioStack);
}




void * fnKernelConectado( void * socketPtr ){


	log_info( logger, "Se conecto el Kernel" );

	Kernel * kernel = malloc( sizeof( Kernel ) );
	kernel->socket = *((int*)socketPtr);
	free(socketPtr);


	if( recibirYProcesarMensajesKernel( kernel ) > 0 ){
		log_info( logger, "El Kernel se desconecto correctamente" );
	}else{
		log_error( logger, "Hubo un problema con el Kernel" );
	}

	//TODO hacer que cuando se desconecte el kernel finalice la UMV

	free( kernel );

	return 0x0000;

}





