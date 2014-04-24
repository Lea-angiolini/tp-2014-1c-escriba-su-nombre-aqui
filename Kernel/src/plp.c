#include "plp.h"
#include "colas.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/sockets.h"
#include "commons/pcb.h"
#include "commons/parser/metadata_program.h"

#define CALCULAR_PRIORIDAD(e,f,t) (5 * e + 3 * f + t)

uint32_t nextProcessId = 1;
int socketUMV;
uint8_t multiprogramacion = 0;
pthread_mutex_t multiprogramacionMutex = PTHREAD_MUTEX_INITIALIZER;
extern t_config *config;
t_log *logplp;

void *IniciarPlp(void *arg) {
	logplp = log_create("log_plp.txt", "KernelPLP", 1, LOG_LEVEL_TRACE);
	log_info(logplp, "Thread iniciado");

#ifdef UMV_ENABLE
	socketUMV = conectar(config_get_string_value(config, "IP_UMV"), config_get_int_value(config, "PUERTO_UMV"), logplp);

	if (socketUMV == -1) {
		log_error(logplp, "No se pudo establecer la conexion con la UMV");
		log_info(logplp, "Thread concluido");
		log_destroy(logplp);
		return NULL ;
	}

	log_info(logplp, "Conectado con la UMV");
#endif

	if (crearServidorNoBloqueante(config_get_int_value(config, "PUERTO_PROG"), nuevoMensaje, logplp) == -1) {
		log_error(logplp, "No se pudo crear el servidor receptor de Programas");
	}

	log_info(logplp, "Thread concluido");
	log_destroy(logplp);
	return NULL ;
}

void MoverNewAReady()
{
	log_info(logplp, "Ordenando la cola New segun algoritmo SJN");
	list_sort(newQueue, sjnAlgorithm);

	log_info(logplp, "Moviendo PCB de la cola New a Ready");
	pthread_mutex_lock(&readyQueueMutex);
	pthread_mutex_lock(&multiprogramacionMutex);
	queue_push(readyQueue, queue_pop(newQueue));
	multiprogramacion++;
	pthread_mutex_unlock(&multiprogramacionMutex);
	pthread_mutex_unlock(&readyQueueMutex);
}

void puedoMoverNewAReady()
{
	log_info(logplp, "Verificando grado de multiprogramacion");

	if(multiprogramacion < config_get_int_value(config, "MULTIPROGRAMACION")) {
		MoverNewAReady();
	}
}

bool sjnAlgorithm(pcb_t *a, pcb_t *b)
{
	return a->prioridad < b->prioridad;
}

void desconexionCliente()
{
	log_info(logplp, "Se ha desconectado un Programa");
	puedoMoverNewAReady();
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

	log_info(logplp, "Nuevo script: %d\n%.*s\n", header.size, scriptSize, script);

	//ansisop preprocesador
	t_medatada_program *scriptMedatada = metadatada_desde_literal(script);
	//t_puntero_instruccion metadata_buscar_etiqueta(scriptMedatada, t_nombre_etiqueta);


	log_info(logplp, "Pidiendole memoria a la UMV para que pueda correr el script ansisop");
	socket_pedirMemoria pedirMemoria;
	pedirMemoria.header.size = sizeof(pedirMemoria);

	pedirMemoria.codeSegmentSize = strlen(script);
	pedirMemoria.etiquetasSegmentSize = scriptMedatada->etiquetas_size;
	pedirMemoria.instruccionesSegmentSize = scriptMedatada->instrucciones_size;

#ifdef UMV_ENABLE
	send(socketUMV, &pedirMemoria, sizeof(pedirMemoria), 0);
#endif

	socket_respuesta respuesta;

#ifdef UMV_ENABLE
	recv(socketUMV, &respuesta, sizeof(respuesta), 0);
#endif

	if(respuesta.valor == true)
	{
		log_info(logplp, "La UMV informo que pudo alojar la memoria necesaria para el script ansisop");
		log_info(logplp, "Enviando a la UMV los datos a guardar en los segmentos");

#ifdef UMV_ENABLE
		send(socketUMV, script, sizeof(script), 0);
		send(socketUMV, scriptMedatada->etiquetas, scriptMedatada->etiquetas_size, 0);
		send(socketUMV, scriptMedatada->instrucciones_serializado, scriptMedatada->instrucciones_size * sizeof(t_intructions), 0);
#endif

		socket_umvpcb umvpcb;
#ifdef UMV_ENABLE
		recv(socketUMV, &socket_umvpcb, sizeof(socket_umvpcb), 0);
#endif

		pcb_t *pcb = malloc(sizeof(pcb_t));

		pcb->id = nextProcessId; nextProcessId++;

		pcb->codeSegment = umvpcb.codeSegment;
		pcb->stackSegment = umvpcb.stackSegment;
		pcb->stackCursor = umvpcb.stackSegment;
		pcb->codeIndex = umvpcb.codeIndex;
		pcb->etiquetaIndex = umvpcb.etiquetaIndex;

		pcb->programCounter = scriptMedatada->instruccion_inicio;
		pcb->contextSize = 0;

		pcb->programaSocket = socket;
		pcb->prioridad = CALCULAR_PRIORIDAD(scriptMedatada->cantidad_de_etiquetas, scriptMedatada->cantidad_de_funciones, scriptMedatada->instrucciones_size);
		pcb->lastErrorCode = 0;

		queue_push(newQueue, pcb);
		log_info(logplp, "Segmentos cargados en la UMV y PCB generada en la cola NEW");

		puedoMoverNewAReady();
	} else {
		log_error(logplp, "La UMV informo que no pudo alojar la memoria necesaria para el script ansisop");
		log_info(logplp, "Informandole a Programa que el script no se puede procesar por el momento");
		socket_msg msg;
		strcpy(msg.msg, "No hay memoria suficiente en este momento para ejecutar este script. Intentelo mas tarde");
		send(socket, &msg, sizeof(msg), 0);

		return false;
	}

	metadata_destruir(scriptMedatada);
	free(script);

	return true;
}

