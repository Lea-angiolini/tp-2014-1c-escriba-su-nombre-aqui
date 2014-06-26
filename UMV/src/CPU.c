#include "CPU.h"


extern t_log *logger;
extern uint32_t retardoUMV;
extern pthread_rwlock_t lockEscrituraLectura;


void fnNuevoCpu(int socketCPU)
{
	log_info( logger, "Se conecto un nuevo CPU" );

	while( recibirYProcesarPedidoCpu(socketCPU) != false );

	log_error(logger, "Un CPU se ha desconectado");
}

bool recibirYProcesarPedidoCpu(int socketCPU)
{
	log_trace(logger, "Esperando pedido de la CPU");
	socket_header header;

	if( recv(socketCPU, &header, sizeof(socket_header), MSG_WAITALL | MSG_PEEK) != sizeof(socket_header) )
		return false;

	usleep(retardoUMV * 1000);

	switch(header.code)
	{
	case 'b':
		return leerMemoria(socketCPU);
	case 'c':
		return escribirMemoria(socketCPU);
	default:
		log_error(logger, "Pedido invalido de la CPU");
		return false;
	}

	return true;
}

bool leerMemoria(int socketCPU)
{
	log_trace(logger, "Procesando solicitud de lectura");
	socket_leerMemoria leerMemoria;

	if( recv(socketCPU, &leerMemoria, sizeof(socket_leerMemoria), MSG_WAITALL) != sizeof(socket_leerMemoria) )
		return false;

	log_debug(logger, "Solicitud de lectura: base = %d, offset = %d, length = %d", leerMemoria.base, leerMemoria.offset, leerMemoria.length);

	Programa* programa = buscarPrograma(leerMemoria.pdi);
	pthread_rwlock_rdlock(&lockEscrituraLectura);
	Segmento* segmento = buscarSegmentoEnProgramaPorVirtual(programa, leerMemoria.base);

	if(segmento == NULL) {
		log_error(logger, "No se encuentra el segmento especificado | UMV/src/cpu.c -> procesarSolicitudLecturaMemoria");
		pthread_rwlock_unlock(&lockEscrituraLectura);
		return false;
	}

	//Respuesta
	char *buffer = malloc(leerMemoria.length);

	socket_RespuestaLeerMemoria respuesta;

	respuesta.header.size = sizeof(socket_RespuestaLeerMemoria) + leerMemoria.length;
	respuesta.status = true;

	uint32_t tamanioParaOperar = tamanioSegmento(segmento) - leerMemoria.offset;

	if(leerMemoria.length > tamanioParaOperar) {
		respuesta.status = false;
		log_error(logger, "Segmentation fault: length: %d, tamanioParaOperar: %d, base: %d, offset: %d | UMV/src/cpu.c -> procesarSolicitudLecturaMemoria", leerMemoria.length, tamanioParaOperar, leerMemoria.base, leerMemoria.offset);
		//return false; notese que de esta forma no desconectamos a esa CPU
	} else {
		memLeer(segmento, buffer, leerMemoria.offset, leerMemoria.length);
		log_debug(logger, "Se leyo la data: %s", buffer);
	}

	pthread_rwlock_unlock(&lockEscrituraLectura);

	if( send(socketCPU, &respuesta, sizeof(socket_RespuestaLeerMemoria), 0) < 0 || send(socketCPU, buffer, leerMemoria.length, 0) < 0 )
	{
		log_error(logger, "Hubo un error al enviar la respuesta a lectura de memoria | UMV/src/cpu.c -> procesarSolicitudLecturaMemoria");
		free(buffer);
		return false;
	}

	log_trace(logger, "Se pudo reponder a la CPU sin problemas, se enviaron %d bytes", respuesta.header.size);

	free(buffer);

	return true;
}

bool escribirMemoria(int socketCPU)
{
	log_trace(logger, "Procesando solicitud de escritura");
	socket_guardarEnMemoria guardarMemoria;

	if( recv(socketCPU, &guardarMemoria, sizeof(socket_guardarEnMemoria), MSG_WAITALL) != sizeof(socket_guardarEnMemoria) )
		return false;

	char *buffer = malloc(guardarMemoria.length);

	if( recv(socketCPU, buffer, guardarMemoria.length, MSG_WAITALL) != guardarMemoria.length )
		return false;

	log_trace(logger, "Solicitud de escritura: base = %d, offset = %d, length = %d", guardarMemoria.base, guardarMemoria.offset, guardarMemoria.length);


	Programa * programa = buscarPrograma(guardarMemoria.pdi);
	pthread_rwlock_rdlock(&lockEscrituraLectura);
	Segmento * segmento = buscarSegmentoEnProgramaPorVirtual(programa, guardarMemoria.base);

	if(segmento == NULL) {
		log_error( logger, "No se encuentra el segmento especificado | UMV/src/cpu.c -> procesarSolicitudEscrituraMemoria");
		pthread_rwlock_unlock(&lockEscrituraLectura);
		free(buffer);
		return false;
	}

	//Respuesta
	socket_RespuestaGuardarEnMemoria respuesta;

	respuesta.header.size = sizeof(socket_RespuestaGuardarEnMemoria);
	respuesta.status = true;

	uint32_t tamanioParaOperar = tamanioSegmento(segmento) - guardarMemoria.offset;

	if(guardarMemoria.length > tamanioParaOperar) {
		respuesta.status = false;
		log_error(logger, "Segmentation fault: length: %d, tamanioParaOperar: %d, base: %d, offset: %d | UMV/src/cpu.c -> procesarSolicitudEscrituraMemoria", guardarMemoria.length, tamanioParaOperar, guardarMemoria.base, guardarMemoria.offset);
		//return false; notese que de esta forma no desconectamos a esa CPU
	}else{
		memCopi(segmento, guardarMemoria.offset, buffer, guardarMemoria.length);
		log_info(logger, "Se guardo la data: %s", buffer);
	}

	pthread_rwlock_unlock(&lockEscrituraLectura);

	if( send(socketCPU, &respuesta, sizeof(socket_RespuestaGuardarEnMemoria), 0 ) < 0) {
		log_error( logger, "La respuesta a escribir en memoria no se ha realizado con exito | UMV/src/cpu.c -> procesarSolicitudEscrituraMemoria");
		free(buffer);
		return false;
	}

	log_info(logger, "La respuesta a escribir en memoria se ha realizado con exito");

	free(buffer);
	return true;
}

