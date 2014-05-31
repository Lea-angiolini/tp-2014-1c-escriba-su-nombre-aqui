#include "systemcalls.h"
#include "io.h"
#include "colas.h"
#include "config.h"

extern t_log *logpcp;

extern sem_t dispatcherReady, dispatcherCpu;

bool syscallIO(int socket)
{
	socket_scIO io;
	socket_pcb spcb;

	if( recv(socket, &io, sizeof(socket_scIO), MSG_WAITALL) != sizeof(socket_scIO) )
		return false;

	if( recv(socket, &spcb, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) )
		return false;

	io_t *disp = dictionary_get(dispositivos, io.identificador);
	data_cola_t *pedido = malloc(sizeof(data_cola_t));

	pedido->pid = spcb.pcb.id;
	pedido->tiempo = io.unidades;

	//Moviendo cpu de cpuExec a cpuReady

	pthread_mutex_lock(&cpuExecQueueMutex);
	cpu_info_t *cpuInfo = list_remove_cpuInfo_by_socketCpu(cpuExecQueue->elements, socket);
	pthread_mutex_unlock(&cpuExecQueueMutex);

	pthread_mutex_lock(&cpuReadyQueueMutex);
	queue_push(cpuReadyQueue, cpuInfo);
	pthread_mutex_unlock(&cpuReadyQueueMutex);

	sem_post(&dispatcherCpu);

	//Actualizando pcb y moviendolo a block

	pthread_mutex_lock(&execQueueMutex);
	pcb_t *pcb = list_remove_pcb_by_pid(execQueue->elements, spcb.pcb.id);
	pthread_mutex_unlock(&execQueueMutex);

	*pcb = spcb.pcb;

	pthread_mutex_lock(&blockQueueMutex);
	queue_push(blockQueue, pcb);
	pthread_mutex_unlock(&blockQueueMutex);

	log_trace(logpcp, "Cargando nuevo trabajo en el dispositivo %s",io.identificador);

	//Cargando nuevo trabajo a la cola
	pthread_mutex_lock(&disp->mutex);
	queue_push(disp->cola, pedido);
	pthread_mutex_unlock(&disp->mutex);

	sem_post(&disp->semaforo);

	return true;
}

bool syscallObtenerValor(int socket)
{
	socket_scObtenerValor sObtenerValor;

	if( recv(socket, &sObtenerValor, sizeof(socket_scObtenerValor), MSG_WAITALL) != sizeof(socket_scObtenerValor) )
		return false;

	int32_t *valor = dictionary_get(variablesCompartidas, sObtenerValor.identificador);
	sObtenerValor.valor = *valor;

	if( send(socket, &sObtenerValor, sizeof(socket_scObtenerValor), 0) < 0 )
		return false;

	return true;
}

bool syscallGrabarValor(int socket)
{
	socket_scGrabarValor sGrabarValor;

	if( recv(socket, &sGrabarValor, sizeof(socket_scGrabarValor), MSG_WAITALL) != sizeof(socket_scGrabarValor) )
		return false;

	int32_t *valor = dictionary_get(variablesCompartidas, sGrabarValor.identificador);
	*valor = sGrabarValor.valor;

	return true;
}

bool syscallWait(int socket)
{
	socket_scWait sWait;

	if( recv(socket, &sWait, sizeof(socket_scWait), MSG_WAITALL) != sizeof(socket_scWait) )
			return false;

	semaforo_t *semaforo = dictionary_get(semaforos, sWait.identificador);
	semaforo->valor -= 1;

	socket_respuesta res;
	res.header.size = sizeof(socket_respuesta);

	if (semaforo->valor < 0)
	{
		//Pedir a la cpu el pcb

		socket_pcb spcb;
		res.valor = false;

		if( send(socket, &res, sizeof(socket_respuesta), 0) < 0 )
			return false;

		if( recv(socket, &spcb, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) )
		    return false;

		uint32_t *pid = malloc(sizeof(uint32_t));
		*pid = spcb.pcb.id;
		queue_push(semaforo->cola, pid);

		//Moviendo cpu de cpuExec a cpuReady

		pthread_mutex_lock(&cpuExecQueueMutex);
		cpu_info_t *cpuInfo = list_remove_cpuInfo_by_socketCpu(cpuExecQueue->elements, socket);
		pthread_mutex_unlock(&cpuExecQueueMutex);

		pthread_mutex_lock(&cpuReadyQueueMutex);
		queue_push(cpuReadyQueue, cpuInfo);
		pthread_mutex_unlock(&cpuReadyQueueMutex);

		sem_post(&dispatcherCpu);

		//Actualizando pcb y moviendolo a block

		pthread_mutex_lock(&execQueueMutex);
		pcb_t *pcb = list_remove_pcb_by_pid(execQueue->elements, spcb.pcb.id);
		pthread_mutex_unlock(&execQueueMutex);

		*pcb = spcb.pcb;

		pthread_mutex_lock(&blockQueueMutex);
		queue_push(blockQueue, pcb);
		pthread_mutex_unlock(&blockQueueMutex);

	}
	else
	{
		res.valor = true;

		if( send(socket, &res, sizeof(socket_respuesta), 0) < 0 )
			return false;
	}



	return true;
}

bool syscallSignal(int socket)
{
	socket_scSignal sSignal;

	if( recv(socket, &sSignal, sizeof(socket_scSignal), MSG_WAITALL) != sizeof(socket_scSignal) )
		return false;

	semaforo_t *semaforo = dictionary_get(semaforos, sSignal.identificador);
	semaforo->valor += 1;

	if (semaforo->valor <= 0)
	{
		//Saco el pcb de la cola de bloqueados y lo pongo en la cola de ready

		uint32_t *pid = queue_pop(semaforo->cola);

		pthread_mutex_lock(&blockQueueMutex);
		pcb_t *pcb = list_remove_pcb_by_pid(blockQueue->elements, *pid);
		pthread_mutex_unlock(&blockQueueMutex);

		pthread_mutex_lock(&readyQueueMutex);
		queue_push(readyQueue, pcb);
		pthread_mutex_unlock(&readyQueueMutex);

		sem_post(&dispatcherReady);

		free(pid);

	}

	return true;
}

bool syscallImprimirTexto(int socket)
{
	socket_imprimirTexto texto;

	if( recv(socket, &texto, sizeof(socket_imprimirTexto), MSG_WAITALL) != sizeof(socket_imprimirTexto) )
			return false;

	socket_msg msg;
	msg.header.size = sizeof(socket_msg);

	strcpy(msg.msg, texto.texto);
	send(texto.programaSocket, &msg, sizeof(socket_msg), 0);

	return true;
}
