#include "Kernel.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "memoria.h"
#include "Programa.h"

#include "commons/log.h"
#include "commons/sockets.h"



extern t_log * logger;


//TODO se podria reusar lo de cpu ??
int recibirYProcesarMensajesKernel( Kernel * kernel ) {

	socket_pedirMemoria * buffer = malloc(sizeof(socket_pedirMemoria));

	while (sizeof(socket_pedirMemoria) == recv(kernel->socket, buffer, sizeof(socket_pedirMemoria), MSG_WAITALL) ) {

			log_info(logger, "Procesando mensaje del Kernel");
			procesarMenssajeKernel(kernel, buffer);
		}

	free(buffer);
	return 1;
}

int procesarMenssajeKernel( Kernel * kernel, socket_pedirMemoria * segmentosAreservar ) {

	bool respuesta = true;
	int memoriaDisponible;
	int tamanioTotalSegmentos = tamanioSegmentos(segmentosAreservar);
	memoriaDisponible = memoriaLibre();

	if (tamanioTotalSegmentos > memoriaDisponible)
		respuesta = false;

	socket_respuesta respuestaSegmentos;
	respuestaSegmentos.header.size = sizeof(respuestaSegmentos);
	respuestaSegmentos.valor = respuesta;

	if (send(kernel->socket, &respuestaSegmentos, sizeof(respuestaSegmentos), 0) == -1) {
		log_error(logger, "No se ha podido enviar respuesta de reservación de segmentos al Kernel");
		return -1;
	}

	if(respuesta == false)
			return 1;


		uint32_t datosPid, datosScript, datosEtiquetas, datosInstrucciones;
		socket_umvpcb  datosSegmentos;

		uint32_t tamanioScript, tamanioEtiquetas, tamanioInstrucciones, tamanioStack;
		tamanioScript			= segmentosAreservar->codeSegmentSize;
		tamanioEtiquetas		= segmentosAreservar->etiquetasSegmentSize;
		tamanioInstrucciones	= segmentosAreservar->instruccionesSegmentSize;
		tamanioStack			= segmentosAreservar->stackSegmentSize;

		void * script			= malloc(tamanioScript);
		void * etiquetas		= malloc(tamanioEtiquetas);
		void * instrucciones	= malloc(tamanioInstrucciones);
		uint32_t * pid			= malloc(sizeof(uint32_t));

		datosPid				= recv(kernel->socket, pid, sizeof(uint32_t), MSG_WAITALL);
		datosScript				= recv(kernel->socket, script, tamanioScript, MSG_WAITALL);
		datosEtiquetas			= recv(kernel->socket, etiquetas, tamanioEtiquetas, MSG_WAITALL);
		datosInstrucciones		= recv(kernel->socket, instrucciones, tamanioInstrucciones, MSG_WAITALL);


		if (
				datosPid != ( sizeof( uint32_t ) )
				|| datosScript != tamanioScript
				|| datosEtiquetas != tamanioEtiquetas
				|| datosInstrucciones != tamanioInstrucciones ) {

			log_error(logger, "No se recibieron los datos de los segmentos correctamente");

			free(script);
			free(etiquetas);
			free(instrucciones);
			free(pid);

			return -1;
		}


		Programa * programa = crearPrograma( *pid, script, etiquetas, instrucciones, tamanioScript, tamanioEtiquetas, tamanioInstrucciones, tamanioStack );
		datosSegmentos		= crearEstructuraParaPCB( programa );

		free(script);
		free(etiquetas);
		free(instrucciones);
		free(pid);

		if ( send(kernel->socket, &datosSegmentos, sizeof(datosSegmentos), 0) < 0 ) {
			log_error( logger, "No se ha podido enviar los datos de los segmentos creados correctamente al Kernel" );
			return -1;
		} else {
			log_info( logger,"Se ha enviado los datos de los segmentos creados correctamente al Kernel" );
			return 1;
		}

}

int tamanioSegmentos(socket_pedirMemoria * segmentosAreservar) {
	return (segmentosAreservar->codeSegmentSize
			+ segmentosAreservar->etiquetasSegmentSize
			+ segmentosAreservar->instruccionesSegmentSize + segmentosAreservar->stackSegmentSize);
}

void * fnKernelConectado(int * socketPtr) {

	log_info(logger, "Se conecto el Kernel");

	Kernel * kernel = malloc(sizeof(Kernel));
	kernel->socket = * socketPtr;
	free(socketPtr);

	if (recibirYProcesarMensajesKernel(kernel) > 0) {
		log_info(logger, "El Kernel se desconecto correctamente");
	} else {
		log_error(logger, "Hubo un problema con el Kernel");
	}

	//TODO hacer que cuando se desconecte el kernel finalice la UMV

	free(kernel);

	return 0x0000;

}

