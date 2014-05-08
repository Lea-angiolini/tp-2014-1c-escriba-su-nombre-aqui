#include "pcp.h"
#include "colas.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <pthread.h>

#include "commons/log.h"
#include "commons/config.h"


t_log *logpcp;
extern t_config *config;

extern uint32_t multiprogramacion;
extern pthread_mutex_t multiprogramacionMutex;

pthread_cond_t dispatcherCond = PTHREAD_COND_INITIALIZER;

t_queue *cpuReadyQueue, *cpuExecQueue;


//char** semaforos = config_get_array_value(config, "SEMAFOROS");
//char** valor_semaforos = config_get_array_value(config, "VALOR_SEMAFORO");
//char** variablesCompartidas = config_get_array_value(config, "VARIABLES_COMPARTIDAS");

void *IniciarPcp(void *arg)
{
	logpcp = log_create("log_pcp.txt", "KernelPCP", 1, LOG_LEVEL_TRACE);
	log_info(logpcp, "Thread iniciado");

	cpuReadyQueue = queue_create();
	cpuExecQueue = queue_create();

	pthread_t dispatcherThread;
	pthread_create(&dispatcherThread, NULL, &Dispatcher, NULL);

	if(crearServidorNoBloqueante(config_get_int_value(config, "PUERTO_CPU"), nuevoMensajeCPU, logpcp) == -1) {
		log_error(logpcp, "No se pudo crear el servidor receptor de CPUs");
	}

	pthread_join(&dispatcherThread, NULL);

	queue_destroy_and_destroy_elements(cpuReadyQueue, free);
	queue_destroy_and_destroy_elements(cpuExecQueue, free);

	log_info(logpcp, "Thread concluido");
	log_destroy(logpcp);
	return NULL;
}

/*
 * Para activar este hilo usar la siguiente instruccion
 * pthread_cond_signal(&dispatcherCond);
 */
void *Dispatcher(void *arg)
{
	log_info(logpcp, "Dispatcher Thread iniciado");

	pthread_mutex_lock(&readyQueueMutex);

	while(1)
	{
		pthread_cond_wait(&dispatcherCond, &readyQueueMutex);
		log_info(logpcp, "Dispatcher invocado");

		/*
		 * Si hay un trabajo en READY y no hay CPU libre no hacer nada
		 * Si no hay trabajos en READY y hay una CPU libre no hacer nada
		 * Si hay un trabajo en READY y hay una CPU libre ponerla a trabajar
		 */
		if( !queue_is_empty(readyQueue) && !queue_is_empty(cpuReadyQueue) )
		{
			log_info(logpcp, "Moviendo PCB de la cola READY a EXEC");
			queue_push(execQueue, queue_pop(readyQueue));

			//Iniciando proceso de ejecucion
			pcb_t *pcb = queue_pop(readyQueue);

			cpu_info_t *cpuInfo = queue_pop(cpuReadyQueue);
			cpuInfo->socketPrograma = pcb->programaSocket;

			//Mandando informacion necesaria para que la CPU pueda empezar a trabajar
			socket_pcb spcb;

			spcb.header.code = 'p';
			spcb.header.size = sizeof(socket_pcb);
			spcb.pcb = *pcb;

			send(cpuInfo->socketCPU, &spcb, sizeof(socket_pcb), 0);

			queue_push(cpuExecQueue, cpuInfo);
		}
	}

	pthread_mutex_unlock(&readyQueueMutex);

	log_info(logpcp, "Dispatcher Thread concluido");

	return NULL;
}

void conexionCPU(int socket)
{
	log_info(logpcp, "Se ha conectado un CPU");

	//Agregandolo a la cpuReadyQueue
	cpu_info_t *cpuInfo = malloc(sizeof(cpu_info_t));
	cpuInfo->socketCPU = socket;
	queue_push(cpuReadyQueue, cpuInfo);

	//Hay que mandar QUANTUM y RETARDO al CPU
	socket_cpucfg cpucfg;

	cpucfg.header.code = 'c';
	cpucfg.header.size = sizeof(socket_cpucfg);

	cpucfg.quantum = config_get_int_value(config, "QUANTUM");
	cpucfg.retardo = config_get_int_value(config, "RETARDO");

	send(socket, &cpucfg, sizeof(socket_cpucfg), 0);

	//Llamanda a dispatcher para ver si hay algun trabajo pendiente para darle al CPU nuevo.
	pthread_cond_signal(&dispatcherCond);
}

void bajarNivelMultiprogramacion()
{
	pthread_mutex_lock(&multiprogramacionMutex);
	multiprogramacion--;
	pthread_mutex_unlock(&multiprogramacionMutex);
}

void desconexionCPU(int socket)
{
	log_info(logpcp, "Se ha desconectado un CPU");

	bool limpiarCpuExec(cpu_info_t *cpuInfo)
	{
		return cpuInfo->socketCPU == socket;
	}

	log_info(logpcp, "Verificando si el CPU desconectado estaba corriendo algun programa");
	cpu_info_t *cpuInfo = list_remove_by_condition(cpuExecQueue->elements, limpiarCpuExec);
	if( cpuInfo != NULL )
	{
		log_error(logpcp, "La CPU desconectada estaba en ejecucion, abortando ejecucion de programa");
		//La cpu estaba en ejecucion, hay que mover la pcb de EXEC a EXIT
		bool limpiarPcb(pcb_t *pcb) {
			return pcb->programaSocket == cpuInfo->socketPrograma;
		}

		log_info(logpcp, "Moviendo PCB de la cola EXEC a EXIT");
		queue_push(exitQueue, list_remove_by_condition(execQueue->elements, limpiarPcb));

		log_info(logpcp, "Informandole a Programa que el script no pudo concluir su ejecucion");

		socket_msg msg;

		strcpy(msg.msg, "El script no pudo concluir su ejecucion debido a la desconexion de un CPU");
		send(cpuInfo->socketPrograma, &msg, sizeof(msg), 0);
		shutdown(cpuInfo->socketPrograma, SHUT_RDWR);

		free(cpuInfo);
		bajarNivelMultiprogramacion();
	}
	else
	{
		//La cpu no estaba en ejecucion, hay que borrarla de la cpuReadyQueue
		bool limpiarCpuReady(cpu_info_t *cpuInfo) {
			return cpuInfo->socketCPU == socket;
		}
		list_remove_and_destroy_by_condition(cpuReadyQueue->elements, limpiarCpuReady, free);
	}
}


bool nuevoMensajeCPU(int socket) {

	if ( recibirYprocesarPedido(socket) == false ) {
		desconexionCPU(socket);
		return false;
	}

	return true;
}

bool recibirYprocesarPedido(int socket)
{
	socket_header header;
	recv(socket, &header, sizeof(header), MSG_WAITALL | MSG_PEEK);

	switch(header.code)
	{
	case 'h': //Conectado
		conexionCPU(socket);
		break;
	}
	return true;
}
