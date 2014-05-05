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

t_log *logplp;
extern t_config *config;
extern pthread_cond_t dispatcherCond;

uint32_t nextProcessId = 1;
uint32_t multiprogramacion = 0;
pthread_mutex_t multiprogramacionMutex = PTHREAD_MUTEX_INITIALIZER;

int socketUMV;



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
	log_info(logplp, "Ordenando la cola NEW segun algoritmo de planificacion SJN");

	bool sjnAlgorithm(pcb_t *a, pcb_t *b)
	{
		return a->prioridad < b->prioridad;
	}
	list_sort(newQueue->elements, sjnAlgorithm);

	log_info(logplp, "Moviendo PCB de la cola NEW a READY");
	pthread_mutex_lock(&readyQueueMutex);
	pthread_mutex_lock(&multiprogramacionMutex);
	queue_push(readyQueue, queue_pop(newQueue));
	multiprogramacion++;
	pthread_mutex_unlock(&multiprogramacionMutex);
	pthread_mutex_unlock(&readyQueueMutex);

	//Llamada a dispatcher del PCP para avisar que hay un nuevo trabajo pendiente
	pthread_cond_signal(&dispatcherCond);
}

void puedoMoverNewAReady()
{
	log_info(logplp, "Verificando grado de multiprogramacion");

	if(multiprogramacion < config_get_int_value(config, "MULTIPROGRAMACION") && !queue_is_empty(newQueue)) {
		MoverNewAReady();
	}
}

void desconexionCliente()
{
	log_info(logplp, "Se ha desconectado un Programa");
	puedoMoverNewAReady();
}


bool nuevoMensaje(int socket) {
	if (recibirYprocesarScript(socket) == false) {
		desconexionCliente();
		return false;
	}

	return true;
}

bool recibirYprocesarScript(int socket) {
	//Pidiendo script ansisop
	socket_header header;

	if( recv(socket, &header, sizeof(header), MSG_WAITALL) <= 0 )
		return false;

	int scriptSize = header.size - sizeof(header);

	char *script = malloc(scriptSize + 1);
	memset(script, 0x00, scriptSize + 1);

	log_info(logplp, "Esperando a recibir un script ansisop");

	if( recv(socket, script, scriptSize, MSG_WAITALL) <= 0 )
		return false;

	log_info(logplp, "Script ansisop recibido");

	//ansisop preprocesador
	t_medatada_program *scriptMedatada = metadatada_desde_literal(script);

	log_info(logplp, "Pidiendole memoria a la UMV para que pueda correr el script ansisop");

	socket_pedirMemoria pedirMemoria;
	pedirMemoria.header.size = sizeof(pedirMemoria);

	pedirMemoria.codeSegmentSize = scriptSize + 1;
	pedirMemoria.etiquetasSegmentSize = scriptMedatada->etiquetas_size;
	pedirMemoria.instruccionesSegmentSize = scriptMedatada->instrucciones_size * sizeof(t_intructions);

#ifdef UMV_ENABLE
	send(socketUMV, &pedirMemoria, sizeof(pedirMemoria), 0);
#endif

	socket_respuesta respuesta;

#ifdef UMV_ENABLE
	recv(socketUMV, &respuesta, sizeof(respuesta), 0);
#endif

	respuesta.valor = true;
	if(respuesta.valor == true)
	{
		log_info(logplp, "La UMV informo que pudo alojar la memoria necesaria para el script ansisop");
		log_info(logplp, "Enviando a la UMV los datos a guardar en los segmentos");

#ifdef UMV_ENABLE
		send(socketUMV, &nextProcessId, sizeof(nextProcessId), 0);
		send(socketUMV, script, pedirMemoria.codeSegmentSize, 0);
		send(socketUMV, scriptMedatada->etiquetas, pedirMemoria.etiquetasSegmentSize, 0);
		send(socketUMV, scriptMedatada->instrucciones_serializado, pedirMemoria.instruccionesSegmentSize, 0);
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

