#include "plp.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <errno.h>

#include "commons/log.h"
#include "commons/sockets.h"
#include "commons/pcb.h"

#define CALCULAR_PRIORIDAD(e,f,t) (5 * e + 3 * f + t)

int socketUMV;
uint8_t multiprogramacion;
t_log *logplp;

void *IniciarPlp(void *arg) {
	logplp = log_create("log_plp.txt", "KernelPLP", 1, LOG_LEVEL_TRACE);
	log_info(logplp, "Thread iniciado");

	/*
	socketUMV = conectar(config_get_string_value(config, "IPUMV"), config_get_int_value(config, "PuertoUMV"), log);

	if (socketUMV == -1) {
		log_error(logplp, "No se pudo establecer la conexion con la UMV");
		log_info(logpcp, "Thread concluido");
		log_destroy(logplp);
		return NULL ;
	}

	log_info(logplp, "Conectado con la UMV");
	*/

	if (crearServidorNoBloqueante(12345, nuevoMensaje, logplp) == -1) {
		log_error(logplp, "No se pudo crear el servidor receptor de Programas");
	}

	log_info(logplp, "Thread concluido");
	log_destroy(logplp);
	return NULL ;
}

#if 0
void MoverNewAReady()
{
	log_info(logplp, "Moviendo PCB de la cola New a Ready");
	list_sort(newQueue->elements, sjnAlgorithm);

	//New->Ready
	queue_push(readyQueue, queue_pop(newQueue));
	multiprogramacion++;
}
#endif

#if 0
void puedoMoverNewAReady()
{
	log_info(logplp, "Verificando grado de multiprogramacion");
	if(multiprogramacion < config(multiprogramacion) {
		//MoverNewAReady();
	}
}
#endif

bool sjnAlgorithm(pcb_t *a, pcb_t *b)
{
	return a->prioridad < b->prioridad;
}

void desconexionCliente()
{
	log_info(logplp, "Se ha desconectado un Programa");
	//puedoMoverNewAReady();
}


bool nuevoMensaje(int socket) {
	socket_header header;
	int rc = recv(socket, &header, sizeof(header), 0);

	if (rc < 0) {
		if (errno != EWOULDBLOCK) {
			return false;
		}
	}

	if (rc == 0 || recibirYprocesarScript(socket, header) == false) {
		desconexionCliente();
		return false;
	}

	return true;
}

bool recibirYprocesarScript(int socket, socket_header header) {
	int scriptSize = header.size - sizeof(header);
	char *script = malloc(scriptSize * sizeof(char));
	int ret;

	log_info(logplp, "Esperando a recibir un script ansisop");
	do {
		ret = recv(socket, script, scriptSize, 0);

		if(ret == 0) //Desconexion
			return false;
	} while(ret != scriptSize);
	log_info(logplp, "Script ansisop recibido");

	printf("Nuevo script: %d %.*s\n", header.size, scriptSize, script);
	free(script);

	//ansisop preprocesador
	/*
	log_info(logplp, "Pidiendole memoria a la UMV para que pueda correr el script ansisop");
	socket_pedirMemoria pedirMemoria;
	pedirMemoria.header.size = sizeof(pedirMemoria);
	pedirMemoria.segmentSize[0] = ;
	pedirMemoria.segmentSize[1] = ;
	pedirMemoria.segmentSize[2] = ;
	pedirMemoria.segmentSize[3] = ;

	send(socketUMV, &pedirMemoria, sizeof(pedirMemoria), 0);

	socket_respuesta respuesta;
	recv(socketUMV, &respuesta, sizeof(respuesta), 0);

	if(socket_respuesta.valor == true)
	{
		log_info(logplp, "La UMV informo que pudo alojar la memoria necesaria para el script ansisop");
		//contruir pcb_t pcb;

		//queue_push(newQueue, pcb);
		//puedoMoverNewAReady();
	} else {
		log_error(logplp, "La UMV informo que no pudo alojar la memoria necesaria para el script ansisop");
		log_info(logplp, "Informandole a Programa que el script no se puede procesar por el momento");
		socket_msg msg;
		strcpy(msg.msg, "No hay memoria suficiente en este momento para ejecutar este script. Intentelo mas tarde");
		send(socket, &msg, sizeof(msg), 0);
		close(socket);
	}
	*/

	return true;
}

