#include "pcp.h"
#include "colas.h"
#include "config.h"
#include "io.h"

t_log *logpcp;

extern uint32_t multiprogramacion;
extern pthread_mutex_t multiprogramacionMutex;

sem_t dispatcherReady, dispatcherCpu;

void *IniciarPcp(void *arg)
{
	logpcp = log_create("log_pcp.txt", "KernelPCP", 1, LOG_LEVEL_TRACE);
	log_info(logpcp, "Thread iniciado");

	pthread_t dispatcherThread;
	pthread_create(&dispatcherThread, NULL, &Dispatcher, NULL);

	if(crearServidorNoBloqueante(config_get_int_value(config, "PUERTO_CPU"), nuevoMensajeCPU, logpcp) == -1) {
		log_error(logpcp, "No se pudo crear el servidor receptor de CPUs");
	}

	pthread_join(&dispatcherThread, NULL);

	log_info(logpcp, "Thread concluido");
	log_destroy(logpcp);
	return NULL;
}

/*
 * Para activar este hilo usar las siguientes instrucciones
 * sem_post(&dispatcherReady);
 * sem_post(&dispatcherCpu);
 */
void *Dispatcher(void *arg)
{
	log_info(logpcp, "Dispatcher Thread iniciado");

	sem_init(&dispatcherReady, 0, 0);
	sem_init(&dispatcherCpu, 0, 0);

	while(1)
	{
		sem_wait(&dispatcherReady);
		sem_wait(&dispatcherCpu);

		log_info(logpcp, "Dispatcher invocado");

		if( queue_is_empty(readyQueue) )
		{
			log_error(logpcp, "Se llamo al dispatcher sin tener procesos en READY");
			sem_post(&dispatcherCpu);
			continue;
		}

		if( queue_is_empty(cpuReadyQueue) )
		{
			log_error(logpcp, "Se llamo al dispatcher sin tener una CPU disponible");
			sem_post(&dispatcherReady);
			continue;
		}

		MoverReadyAExec();
	}

	sem_destroy(&dispatcherReady);
	sem_destroy(&dispatcherCpu);

	log_info(logpcp, "Dispatcher Thread concluido");

	return NULL;
}

void MoverReadyAExec()
{
	log_info(logpcp, "Moviendo PCB de la cola READY a EXEC");

	//Iniciando proceso de ejecucion
	pthread_mutex_lock(&readyQueueMutex);
	pcb_t *pcb = queue_pop(readyQueue);
	pthread_mutex_unlock(&readyQueueMutex);

	pthread_mutex_lock(&execQueueMutex);
	queue_push(execQueue, pcb);
	pthread_mutex_unlock(&execQueueMutex);

	pthread_mutex_lock(&cpuReadyQueueMutex);
	cpu_info_t *cpuInfo = queue_pop(cpuReadyQueue);
	pthread_mutex_unlock(&cpuReadyQueueMutex);

	cpuInfo->socketPrograma = pcb->programaSocket;

	//Mandando informacion necesaria para que la CPU pueda empezar a trabajar
	socket_pcb spcb;

	spcb.header.code = 'p';
	spcb.header.size = sizeof(socket_pcb);
	spcb.pcb = *pcb;

	pthread_mutex_lock(&cpuExecQueueMutex);
	queue_push(cpuExecQueue, cpuInfo);
	pthread_mutex_unlock(&cpuExecQueueMutex);

	if( send(cpuInfo->socketCPU, &spcb, sizeof(socket_pcb), 0) <= 0 )
		desconexionCPU(cpuInfo->socketCPU);
}

bool conexionCPU(int socket)
{
	log_info(logpcp, "Se ha conectado un CPU");

	socket_header header;

	if( recv(socket, &header, sizeof(socket_header), MSG_WAITALL) != sizeof(socket_header) )
		return false;

	//Agregandolo a la cpuReadyQueue
	cpu_info_t *cpuInfo = malloc(sizeof(cpu_info_t));
	cpuInfo->socketCPU = socket;

	pthread_mutex_lock(&cpuReadyQueueMutex);
	queue_push(cpuReadyQueue, cpuInfo);
	pthread_mutex_unlock(&cpuReadyQueueMutex);

	//Hay que mandar QUANTUM y RETARDO al CPU
	socket_cpucfg cpucfg;

	cpucfg.header.code = 'c';
	cpucfg.header.size = sizeof(socket_cpucfg);

	cpucfg.quantum = config_get_int_value(config, "QUANTUM");
	cpucfg.retardo = config_get_int_value(config, "RETARDO");

	if( send(socket, &cpucfg, sizeof(socket_cpucfg), 0) <= 0 )
		return false;

	//Llamanda a dispatcher para ver si hay algun trabajo pendiente para darle al CPU nuevo.
	sem_post(&dispatcherCpu);

	return true;
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

	bool matchearCPU(cpu_info_t *cpuInfo)
	{
		return cpuInfo->socketCPU == socket;
	}

	log_info(logpcp, "Verificando si el CPU desconectado estaba corriendo algun programa");

	pthread_mutex_lock(&cpuExecQueueMutex);
	cpu_info_t *cpuInfo = list_remove_by_condition(cpuExecQueue->elements, matchearCPU);
	pthread_mutex_unlock(&cpuExecQueueMutex);

	if( cpuInfo != NULL )
	{
		log_error(logpcp, "La CPU desconectada estaba en ejecucion, abortando ejecucion de programa");

		bool matchearPCB(pcb_t *pcb) {
			return pcb->programaSocket == cpuInfo->socketPrograma;
		}

		log_info(logpcp, "Moviendo PCB de la cola EXEC a EXIT");
		queue_push(exitQueue, cpuInfo);

		log_info(logpcp, "Informandole a Programa que el script no pudo concluir su ejecucion");

		socket_msg msg;

		strcpy(msg.msg, "El script no pudo concluir su ejecucion debido a la desconexion de un CPU");
		send(cpuInfo->socketPrograma, &msg, sizeof(socket_msg), 0);
		shutdown(cpuInfo->socketPrograma, SHUT_RDWR);

		bajarNivelMultiprogramacion();
	}
	else
	{
		log_info(logpcp, "La CPU desconectada no se encontraba en ejecucion");
		pthread_mutex_lock(&cpuReadyQueueMutex);
		list_remove_and_destroy_by_condition(cpuReadyQueue->elements, matchearCPU, free);
		pthread_mutex_unlock(&cpuReadyQueueMutex);
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
	if( recv(socket, &header, sizeof(header), MSG_WAITALL | MSG_PEEK) != sizeof(socket_header) )
		return false;

	switch(header.code)
	{
	case 'h': //Conectado
		return conexionCPU(socket);
	case 'i': //SC: IO
		return syscallIO(socket);
	case 'o': //SC: Obtener valor
		return syscallObtenerValor(socket);
	case 'g': //SC: Grabar valor
		return syscallGrabarValor(socket);
	case 'w': //SC: Wait
		return syscallWait(socket);
	case 's': //SC: Signal
		return syscallSignal(socket);
	case 'p': //Termino Quantum
		return terminoQuantumCPU(socket);
	}
	return true;
}

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

	bool matchearCPU(cpu_info_t *cpuInfo){
		return cpuInfo->socketCPU == socket;
	}

	pthread_mutex_lock(&cpuExecQueueMutex);
	cpu_info_t *cpuInfo = list_remove_by_condition(cpuExecQueue->elements, matchearCPU);
	pthread_mutex_unlock(&cpuExecQueueMutex);

	pthread_mutex_lock(&cpuReadyQueueMutex);
	queue_push(cpuReadyQueue, cpuInfo);
	pthread_mutex_unlock(&cpuReadyQueueMutex);

	sem_post(&dispatcherCpu);

	//Actualizando pcb y moviendolo a block

	bool matchearPCB(pcb_t *pcb) {
		return pcb->id == spcb.pcb.id;
	}

	pthread_mutex_lock(&execQueueMutex);
	pcb_t *pcb = list_remove_by_condition(execQueue->elements, matchearPCB);
	pthread_mutex_unlock(&execQueueMutex);

	*pcb = spcb.pcb;

	pthread_mutex_lock(&blockQueueMutex);
	queue_push(blockQueue, pcb);
	pthread_mutex_unlock(&blockQueueMutex);

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

	uint32_t *valor = dictionary_get(variablesCompartidas, sObtenerValor.identificador);
	sObtenerValor.valor = *valor;

	if( send(socket, &sObtenerValor, sizeof(socket_scObtenerValor), 0) != sizeof(socket_scObtenerValor) )
		return false;

	return true;
}

bool syscallGrabarValor(int socket)
{
	socket_scGrabarValor sGrabarValor;

	if( recv(socket, &sGrabarValor, sizeof(socket_scGrabarValor), MSG_WAITALL) != sizeof(socket_scGrabarValor) )
		return false;

	uint32_t *valor = dictionary_get(variablesCompartidas, sGrabarValor.identificador);
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

	if (semaforo->valor < 0)
	{
		//Pedir a la cpu el pcb

		socket_pcb spcb;
		res.valor = false;

		if( send(socket, &res, sizeof(socket_respuesta), 0) < 0 )
			return false;

		if( recv(socket, &spcb, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) )
		    return false;

		uint32_t *id = malloc(sizeof(uint32_t));
		*id = spcb.pcb.id;
		queue_push(semaforo->cola, id);

		//Moviendo cpu de cpuExec a cpuReady

		bool matchearCPU(cpu_info_t *cpuInfo){
			return cpuInfo->socketCPU == socket;
		}

		pthread_mutex_lock(&cpuExecQueueMutex);
		cpu_info_t *cpuInfo = list_remove_by_condition(cpuExecQueue->elements, matchearCPU);
		pthread_mutex_unlock(&cpuExecQueueMutex);

		pthread_mutex_lock(&cpuReadyQueueMutex);
		queue_push(cpuReadyQueue, cpuInfo);
		pthread_mutex_unlock(&cpuReadyQueueMutex);

		sem_post(&dispatcherCpu);

		//Actualizando pcb y moviendolo a block

		bool matchearPCB(pcb_t *pcb) {
			return pcb->id == spcb.pcb.id;
		}

		pthread_mutex_lock(&execQueueMutex);
		pcb_t *pcb = list_remove_by_condition(execQueue->elements, matchearPCB);
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

		uint32_t *id = queue_pop(semaforo->cola);

		bool matchearPCB(pcb_t *pcb){
			return pcb->id == *id;
		}

		pthread_mutex_lock(&blockQueueMutex);
		pcb_t *pcb = list_remove_by_condition(blockQueue->elements, matchearPCB);
		pthread_mutex_unlock(&blockQueueMutex);

		pthread_mutex_lock(&readyQueueMutex);
		queue_push(readyQueue, pcb);
		pthread_mutex_unlock(&readyQueueMutex);

		sem_post(&dispatcherReady);
		//  Liberar memoria de id??
	}

	return true;
}

bool terminoQuantumCPU(int socket)
{
	socket_pcb spcb;

	if( recv(socket, &spcb, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) )
		return false;

	//Sacar cpu de la cpuExecQueue y pasarla a cpuReadyQueue
	bool matchearCPU(cpu_info_t *cpuInfo){
		return cpuInfo->socketCPU == socket;
	}

	pthread_mutex_lock(&cpuExecQueueMutex);
	cpu_info_t *cpuInfo = list_remove_by_condition(cpuExecQueue->elements, matchearCPU);
	pthread_mutex_unlock(&cpuExecQueueMutex);

	pthread_mutex_lock(&cpuReadyQueueMutex);
	queue_push(cpuReadyQueue, cpuInfo);
	pthread_mutex_unlock(&cpuReadyQueueMutex);

	sem_post(&dispatcherCpu);


	bool matchearPCB(pcb_t *pcb) {
		return spcb.pcb.id == pcb->id;
	}

	pthread_mutex_lock(&execQueueMutex);
	pcb_t *pcb = list_remove_by_condition(execQueue->elements, matchearPCB);
	pthread_mutex_unlock(&execQueueMutex);

	*pcb = spcb.pcb;

	pthread_mutex_lock(&readyQueueMutex);
	queue_push(readyQueue, pcb);
	pthread_mutex_unlock(&readyQueueMutex);

	sem_post(&dispatcherReady);

	return true;
}
