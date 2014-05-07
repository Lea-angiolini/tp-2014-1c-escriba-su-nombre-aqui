#include "Kernel.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "memoria.h"
#include "Programa.h"

#include "commons/log.h"
#include "commons/sockets.h"


#define BUFF_SIZE 1024

extern t_log * logger;


//TODO se podria reusar lo de cpu ??
int recibirYProcesarMensajesKernel(Kernel * kernel) {
	int nbytesRecibidos;
	socket_pedirMemoria * buffer = malloc(sizeof(socket_pedirMemoria));

	while (1) {

		nbytesRecibidos = recv(kernel->socket, &buffer, BUFF_SIZE, 0);

		if (nbytesRecibidos > 0) {

			procesarMenssajeKernel(kernel, buffer);
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

int procesarMenssajeKernel(Kernel * kernel,
		socket_pedirMemoria * segmentosAreservar) {

	bool respuesta = true;
	int memoriaDisponible;
	int tamanioTotalSegmentos = tamanioSegmentos(segmentosAreservar);
	memoriaDisponible = memoriaLibre();

	if (tamanioTotalSegmentos > memoriaDisponible)
		respuesta = false;

	socket_respuesta respuestaSegmentos;
	respuestaSegmentos.header.size = sizeof(respuestaSegmentos);
	respuestaSegmentos.valor = respuesta;

	if (send(kernel->socket, &respuestaSegmentos, sizeof(respuestaSegmentos), 0)
			== -1) {
		log_error(logger,
				"No se ha podido enviar respuesta de reservación de segmentos al Kernel");
	}

	if (respuesta == true) {

		uint32_t datosPid, datosScript, datosEtiquetas, datosInstrucciones;

		socket_umvpcb  datosSegmentos;
		uint32_t tamanioScript, tamanioEtiquetas, tamanioInstrucciones;

		tamanioScript = segmentosAreservar->codeSegmentSize;
		tamanioEtiquetas = segmentosAreservar->etiquetasSegmentSize;
		tamanioInstrucciones = segmentosAreservar->instruccionesSegmentSize;

		void * script = malloc(tamanioScript);
		void * etiquetas = malloc(tamanioEtiquetas);
		void * instrucciones = malloc(tamanioInstrucciones);
		uint32_t * pid = malloc(sizeof(uint32_t));

		datosPid = recv(kernel->socket, pid, sizeof(int), 0);
		datosScript = recv(kernel->socket, script, tamanioScript, 0);
		datosEtiquetas = recv(kernel->socket, etiquetas, tamanioEtiquetas, 0);
		datosInstrucciones = recv(kernel->socket, instrucciones,
				tamanioInstrucciones, 0);

		if (datosPid != (sizeof(int)) || datosScript != tamanioScript
				|| datosEtiquetas != tamanioEtiquetas
				|| datosInstrucciones != tamanioInstrucciones) {
			log_error(logger,
					"No se recibieron los datos de los segmentos correctamente");

			free(script);
			free(etiquetas);
			free(instrucciones);
			free(pid);

			return -1;
		}

		Programa * programa = crearPrograma(*pid, script, etiquetas, instrucciones, segmentosAreservar);
		datosSegmentos = crearEstructuraParaPCB( programa);

		free(script);
		free(etiquetas);
		free(instrucciones);
		free(pid);

		if (send(kernel->socket, &datosSegmentos, sizeof(datosSegmentos), 0)
				< 0) {
			log_error(logger,
					"No se ha podido enviar los datos de los segmentos creados correctamente al Kernel");
		} else {
			log_info(logger,
					"Se ha enviado los datos de los segmentos creados correctamente al Kernel");
			return 1;
		}
	}
	return 1;
}

int tamanioSegmentos(socket_pedirMemoria * segmentosAreservar) {
	return (segmentosAreservar->codeSegmentSize
			+ segmentosAreservar->etiquetasSegmentSize
			+ segmentosAreservar->instruccionesSegmentSize + segmentosAreservar->stackSegmentSize);
}

void * fnKernelConectado(void * socketPtr) {

	log_info(logger, "Se conecto el Kernel");

	Kernel * kernel = malloc(sizeof(Kernel));
	kernel->socket = *((int*) socketPtr);
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

