#include "Kernel.h"



extern t_log * logger;
extern FILE *mensajesUMV;
extern pthread_t threadConexiones;
extern uint32_t retardoUMV;


uint32_t recibirYProcesarMensajesKernel( Kernel * kernel ) {

	socket_header header;
	uint32_t todoSaleBien = 1;

	while (todoSaleBien) {
		todoSaleBien = recv(kernel->socket, &header, sizeof(socket_header), MSG_WAITALL | MSG_PEEK);
		log_info( logger, "Recibido un mensaje del Kernel");
		fprintf( mensajesUMV, "Recibido un mensaje del Kernel\n");
		usleep(retardoUMV * 1000);

		if( todoSaleBien == sizeof(socket_header) ){
			log_info( logger, "Procesando mensaje del Kernel");
			fprintf( mensajesUMV, "Procesando mensaje del Kernel\n");
			switch (header.code)
			{
			case 'p' :
					{
						socket_pedirMemoria * pedidoMemoria = malloc( sizeof( socket_pedirMemoria));
						if( recv( kernel->socket, pedidoMemoria, sizeof(socket_pedirMemoria), MSG_WAITALL) != sizeof( socket_pedirMemoria)){
							free( pedidoMemoria);
							return 0;
						}
						reservarSegmentosParaPrograma( kernel, pedidoMemoria);
						free( pedidoMemoria);
						break;
					}
			case 'b':
					{
						socket_borrarMemoria * borradoMemoria = malloc( sizeof( socket_borrarMemoria));
						if( recv( kernel->socket, borradoMemoria, sizeof(socket_borrarMemoria), MSG_WAITALL) != sizeof( socket_borrarMemoria)){
						free( borradoMemoria);
						return 0;
						}
						borrarPrograma( borradoMemoria);
						free( borradoMemoria);
						break;
					}
			}
		}else{
			todoSaleBien = 0;
		}
	}

	return 0;
}

uint32_t reservarSegmentosParaPrograma( Kernel * kernel, socket_pedirMemoria * segmentosAreservar ) {

	bool respuesta = true;
	uint32_t memoriaDisponible;
	uint32_t tamanioTotalSegmentos = tamanioSegmentos(segmentosAreservar);
	memoriaDisponible = memoriaLibre();

	if (tamanioTotalSegmentos > memoriaDisponible){
		log_error( logger, "Memory Overload/ No se ha podido reservar segmentos enviados por el Kernel");
		fprintf( mensajesUMV, "Memory Overload/ No se ha podido reservar segmentos enviados por el Kernel\n");
		respuesta = false;
	}

	socket_respuesta respuestaSegmentos;
	respuestaSegmentos.header.size = sizeof(respuestaSegmentos);
	respuestaSegmentos.valor = respuesta;

	if (send(kernel->socket, &respuestaSegmentos, sizeof(respuestaSegmentos), 0) == -1) {
		log_error( logger, "No se ha podido enviar respuesta de reservación de segmentos al Kernel");
		fprintf( mensajesUMV, "No se ha podido enviar respuesta de reservación de segmentos al Kernel\n");
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

			log_error( logger, "No se recibieron los datos de los segmentos correctamente");
			fprintf( mensajesUMV, "No se recibieron los datos de los segmentos correctamente\n");
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
			fprintf( mensajesUMV, "No se ha podido enviar los datos de los segmentos creados correctamente al Kernel\n");
			return -1;
		} else {
			log_info( logger,"Se ha enviado los datos de los segmentos creados correctamente al Kernel" );
			fprintf( mensajesUMV, "Se ha enviado los datos de los segmentos creados correctamente al Kernel\n");
			return 1;
		}

}

uint32_t borrarPrograma( socket_borrarMemoria * programaAborrar){

	log_info( logger, "Procesando solicitud del Kernel de borrado de programa");
	fprintf( mensajesUMV, "Procesando solicitud del Kernel de borrado de programa\n");
	Programa * programa = buscarPrograma( programaAborrar->pid);
	if( programa == NULL){
		log_error( logger, "El programa solicitado por el Kernel para su borrado no existe en memoria");
		fprintf( mensajesUMV, "El programa solicitado por el Kernel para su borrado no existe en memoria\n");
	}

	removerPIDactivoACPU( programa->pid);
	return destruirPrograma( programa );
}

uint32_t tamanioSegmentos(socket_pedirMemoria * segmentosAreservar) {
	return (segmentosAreservar->codeSegmentSize
			+ segmentosAreservar->etiquetasSegmentSize
			+ segmentosAreservar->instruccionesSegmentSize + segmentosAreservar->stackSegmentSize);
}


void  fnKernelConectado(int socket) {

	log_info( logger, "Se conecto el Kernel");
	fprintf( mensajesUMV, "Se conecto el Kernel\n");

	Kernel * kernel = malloc(sizeof(Kernel));
	kernel->socket = socket;

	if (recibirYProcesarMensajesKernel(kernel) == 0)
		log_info( logger, "El Kernel se ha desconectado");
		fprintf( mensajesUMV, "El Kernel se ha desconectado\n");

	pthread_cancel( threadConexiones );

	free(kernel);
}

