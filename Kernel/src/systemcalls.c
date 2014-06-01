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

	log_debug(logpcp, "CPU: %d, mando nuevo trabajo de IO del dispositivo: %s", socket, io.identificador);
	log_trace(logpcp, "Obteniendo dispositivo desde el diccionario de dispositivos");

	io_t *disp = dictionary_get(dispositivos, io.identificador);
	data_cola_t *pedido = malloc(sizeof(data_cola_t));

	pedido->pid = spcb.pcb.id;
	pedido->tiempo = io.unidades;

	moverCpuAReady(sacarCpuDeExec(socket));

	sem_post(&dispatcherCpu);

	pcb_t *pcb = sacarDeExec(spcb.pcb.id);
	*pcb = spcb.pcb;
	moverABlock(pcb);

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

	log_debug(logpcp, "CPU: %d, pidio el valor de la variable: %s", socket, sObtenerValor.identificador);
	log_trace(logpcp, "Obteniendo valor desde el diccionario de variables compartidas");

	int32_t *valor = dictionary_get(variablesCompartidas, sObtenerValor.identificador);
	sObtenerValor.valor = *valor;

	if( send(socket, &sObtenerValor, sizeof(socket_scObtenerValor), 0) < 0 )
		return false;

	log_debug(logpcp, "Se envio el valor: %d, de la variable: %s al CPU: %d", sObtenerValor.valor, sObtenerValor.identificador, socket);

	return true;
}

bool syscallGrabarValor(int socket)
{
	socket_scGrabarValor sGrabarValor;

	if( recv(socket, &sGrabarValor, sizeof(socket_scGrabarValor), MSG_WAITALL) != sizeof(socket_scGrabarValor) )
		return false;

	log_debug(logpcp, "CPU: %d, pidio grabar el valor: %d en la variable: %s", socket, sGrabarValor.valor, sGrabarValor.identificador);
	log_trace(logpcp, "Grabando valor en el diccionario de variables compartidas");

	int32_t *valor = dictionary_get(variablesCompartidas, sGrabarValor.identificador);
	*valor = sGrabarValor.valor;

	return true;
}

bool syscallWait(int socket)
{
	socket_scWait sWait;

	if( recv(socket, &sWait, sizeof(socket_scWait), MSG_WAITALL) != sizeof(socket_scWait) )
			return false;

	log_debug(logpcp, "CPU: %d, hizo wait en el semaforo: %s", socket, sWait.identificador);
	log_trace(logpcp, "Decrementando semaforo en el diccionario de semaforos");

	semaforo_t *semaforo = dictionary_get(semaforos, sWait.identificador);
	semaforo->valor -= 1;

	socket_respuesta res;
	res.header.size = sizeof(socket_respuesta);

	if (semaforo->valor < 0)
	{
		log_trace(logpcp, "El semaforo quedo con el valor negativo: %d, enviando respuesta y pidiendo PCB",semaforo->valor);

		socket_pcb spcb;
		res.valor = false;

		if( send(socket, &res, sizeof(socket_respuesta), 0) < 0 )
			return false;

		if( recv(socket, &spcb, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) )
		    return false;

		log_trace(logpcp, "PCB recibida. Agregandola a la cola del semaforo");

		uint32_t *pid = malloc(sizeof(uint32_t));
		*pid = spcb.pcb.id;
		queue_push(semaforo->cola, pid);

		//Moviendo cpu de cpuExec a cpuReady

		moverCpuAReady(sacarCpuDeExec(socket));

		sem_post(&dispatcherCpu);

		//Actualizando pcb y moviendolo a block

		pcb_t *pcb = sacarDeExec(spcb.pcb.id);
		*pcb = spcb.pcb;
		moverABlock(pcb);

	}
	else
	{
		log_trace(logpcp, "El semaforo quedo con el valor positivo: %d, enviando respuesta para que el script continue",semaforo->valor);

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

	log_debug(logpcp, "CPU: %d, hizo signal en el semaforo: %s", socket, sSignal.identificador);
	log_trace(logpcp, "Incrementando semaforo en el diccionario de semaforos");

	semaforo_t *semaforo = dictionary_get(semaforos, sSignal.identificador);
	semaforo->valor += 1;

	if (semaforo->valor <= 0)
	{
		//Saco el pcb de la cola de bloqueados y lo pongo en la cola de ready
		log_trace(logpcp, "Habia PCB bloquedas, moviendo una a ready. Quedan: %d",abs(semaforo->valor));

		uint32_t *pid = queue_pop(semaforo->cola);

		moverAReady(sacarDeBlock(*pid));

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

	log_debug(logpcp, "CPU: %d, envia un mensaje al Programa: %d", socket, texto.programaSocket);
	socket_msg msg;
	msg.header.size = sizeof(socket_msg);

	strcpy(msg.msg, texto.texto);
	send(texto.programaSocket, &msg, sizeof(socket_msg), 0);

	log_trace(logpcp, "Mensaje enviado al Programa: %d", texto.programaSocket);

	return true;
}
