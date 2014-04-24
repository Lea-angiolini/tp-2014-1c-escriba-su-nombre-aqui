#include "pcp.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/sockets.h"

t_log *logpcp;
extern t_config *config;
extern uint8_t multiprogramacion;
extern pthread_mutex_t multiprogramacionMutex;

//config_get_int_value(config, "QUANTUM");
//config_get_int_value(config, "RETARDO");
//char** semaforos = config_get_array_value(config, "SEMAFOROS");
//char** valor_semaforos = config_get_array_value(config, "VALOR_SEMAFORO");
//char** variablesCompartidas = config_get_array_value(config, "VARIABLES_COMPARTIDAS");

void *IniciarPcp(void *arg)
{
	logpcp = log_create("log_pcp.txt", "KernelPCP", 1, LOG_LEVEL_TRACE);
	log_info(logpcp, "Thread iniciado");

	if(crearServidorNoBloqueante(config_get_int_value(config, "PUERTO_CPU"), nuevoMensajeCPU, logpcp) == -1) {
		log_error(logpcp, "No se pudo crear el servidor receptor de Programas");
	}

	log_info(logpcp, "Thread concluido");
	log_destroy(logpcp);
	return NULL;
}

void desconexionCPU()
{
	log_info(logpcp, "Se ha desconectado un CPU");
}


bool nuevoMensajeCPU(int socket) {
	socket_header header;
	int rc = recv(socket, &header, sizeof(header), 0);

	if (rc < 0) {
		if (errno != EWOULDBLOCK) {
			return false;
		}
	}

	if (rc == 0 || recibirYprocesarPedido(socket, header) == false) {
		desconexionCPU();
		return false;
	}

	return true;
}

bool recibirYprocesarPedido(int socket, socket_header header)
{

	return true;
}
