#include "umv.h"
#include "config.h"

extern t_log *logplp;

extern sem_t semKernel;

int socketUMV;

bool conectarUMV()
{
	socketUMV = conectar(config_get_string_value(config, "IP_UMV"), config_get_int_value(config, "PUERTO_UMV"), logplp);

	if (socketUMV == -1) {
		log_error(logplp, "No se pudo establecer la conexion con la UMV");
		return false;
	}

	log_info(logplp, "Conectado con la UMV");

	socket_header handshake;

	handshake.size = sizeof(socket_header);
	handshake.code = 'k'; //Kernel

	if( send(socketUMV, &handshake, sizeof(socket_header), 0) <= 0 )
	{
		log_error(logplp, "No se puedo enviar Handshake a la UMV");
		sem_post(&semKernel);
		return false;
	}

	return true;
}

bool solicitarCreacionSegmentos(uint32_t scriptSize, t_metadata_program *scriptMetadata)
{
	log_info(logplp, "Pidiendole memoria a la UMV para que pueda correr el script ansisop");

	socket_pedirMemoria pedirMemoria;
	pedirMemoria.header.size = sizeof(pedirMemoria);

	pedirMemoria.codeSegmentSize = scriptSize + 1;
	pedirMemoria.stackSegmentSize = config_get_int_value(config, "STACK_SIZE");
	pedirMemoria.etiquetasSegmentSize = scriptMetadata->etiquetas_size;
	pedirMemoria.instruccionesSegmentSize = scriptMetadata->instrucciones_size * sizeof(t_intructions);

	if( send(socketUMV, &pedirMemoria, sizeof(socket_pedirMemoria), 0) <= 0 ){
		log_error(logplp, "No se puedo pedir memoria a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return true;
}

bool respuestaCreacionSegmentos()
{
	socket_respuesta respuesta;

	if( recv(socketUMV, &respuesta, sizeof(socket_respuesta), MSG_WAITALL) != sizeof(socket_respuesta) ){
		log_error(logplp, "No se recibio respuesta de la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return respuesta.valor;
}

bool enviarSegmentos(uint32_t pid, char *script, uint32_t scriptSize, t_metadata_program *scriptMetadata)
{
	if( send(socketUMV, &pid, sizeof(uint32_t), 0) <= 0 ){
		log_error(logplp, "No se pudo enviar nextProcessId a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	if( send(socketUMV, script, scriptSize + 1, 0) < 0 ){
		log_error(logplp, "No se pudo enviar script a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	if( send(socketUMV, scriptMetadata->etiquetas, scriptMetadata->etiquetas_size, 0) < 0 ){
		log_error(logplp, "No se pudo enviar etiquetas a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	if( send(socketUMV, scriptMetadata->instrucciones_serializado, scriptMetadata->instrucciones_size * sizeof(t_intructions), 0) < 0 ){
		log_error(logplp, "No se puedo enviar instrucciones serializado a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return true;
}

bool respuestaSegmentos(socket_umvpcb *umvpcb)
{
	if( recv(socketUMV, umvpcb, sizeof(socket_umvpcb), MSG_WAITALL) != sizeof(socket_umvpcb) ){
		log_error(logplp, "No se recibio las direcciones de los segmentos de la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return true;
}
