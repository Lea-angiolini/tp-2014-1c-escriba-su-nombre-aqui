#include "Kernel.h"



extern t_log * logger;
extern pthread_t threadConexiones;
extern uint32_t retardoUMV;


uint32_t recibirYProcesarMensajesKernel( Kernel * kernel ) {

	socket_pedirMemoria * buffer = malloc(sizeof(socket_pedirMemoria));
	uint32_t todoSaleBien = 1;

	while (todoSaleBien) {
		todoSaleBien = recv(kernel->socket, buffer, sizeof(socket_pedirMemoria), MSG_WAITALL);
		log_info(logger, "Recibido un mensaje del Kernel, esperando retardo para procesar...");
		usleep(retardoUMV * 1000);
		if(sizeof(socket_pedirMemoria) ==  todoSaleBien){
			log_info(logger, "Procesando mensaje del Kernel");
			procesarMenssajeKernel(kernel, buffer);
		}else{
			free(buffer);
			return 0;
		}
	}
	free(buffer);
	return 1;
}

uint32_t procesarMenssajeKernel( Kernel * kernel, socket_pedirMemoria * segmentosAreservar ) {

	bool respuesta = true;
	uint32_t memoriaDisponible;
	uint32_t tamanioTotalSegmentos = tamanioSegmentos(segmentosAreservar);
	memoriaDisponible = memoriaLibre();

	if (tamanioTotalSegmentos > memoriaDisponible){
		log_error( logger, "Memory Overload/ No se ha podido reservar segmentos enviados por el Kernel");
		respuesta = false;
	}

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

uint32_t tamanioSegmentos(socket_pedirMemoria * segmentosAreservar) {
	return (segmentosAreservar->codeSegmentSize
			+ segmentosAreservar->etiquetasSegmentSize
			+ segmentosAreservar->instruccionesSegmentSize + segmentosAreservar->stackSegmentSize);
}


void  fnKernelConectado(int socket) {

	log_info(logger, "Se conecto el Kernel");

	Kernel * kernel = malloc(sizeof(Kernel));
	kernel->socket = socket;

	if (recibirYProcesarMensajesKernel(kernel) == 0)
		log_error(logger, "El Kernel se ha desconectado");

	pthread_cancel( threadConexiones );

	free(kernel);
}

