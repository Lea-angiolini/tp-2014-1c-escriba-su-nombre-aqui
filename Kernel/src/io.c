#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "io.h"
#include "colas.h"
#include "config.h"
#include "commons/pcb.h"


void *hilo_io(void *ptr){
	io_t *parametros = (io_t *) ptr;

	while(1){
		sem_wait(&parametros->semaforo);

		//saco el primer elemento de la cola del dispositivo para activarlo
		pthread_mutex_lock(&parametros->mutex);
		data_cola_t *orden_activa = queue_pop(parametros->cola);
		pthread_mutex_unlock(&parametros->mutex);

		//aplico el retardo
		usleep((orden_activa->tiempo) * (parametros->retardo));

		//funcion usada como condicion para buscar el pcb correspondiente en la blockQueue
		bool matchearPCB (pcb_t *pcb){
			return pcb->id == orden_activa->pid;
		}
	
		//Saco el pcb de la cola de bloqueados y la pongo en la cola de ready
		pthread_mutex_lock(&blockQueueMutex);
		pthread_mutex_lock(&readyQueueMutex);
		queue_push(readyQueue, list_remove_by_condition(blockQueue->elements, matchearPCB));
		pthread_mutex_unlock(&blockQueueMutex);
		pthread_mutex_unlock(&readyQueueMutex);

		free(orden_activa);
	}

	return 0;
}

io_t *crear_registro(char* hioRetardo){

	//creo el registro del dispositivo io que va incluido en
	//la lista de dispositivos
	io_t *nuevo_registro = malloc(sizeof(io_t));
	
	//le asigno los datos al registro
	nuevo_registro->retardo = atoi(hioRetardo);
	nuevo_registro->cola = queue_create();

	pthread_mutex_init(&nuevo_registro->mutex, NULL);
	sem_init(&nuevo_registro->semaforo, 0, 0);
	pthread_create(&(nuevo_registro->thread), NULL, &hilo_io, (void *)nuevo_registro);

	return nuevo_registro;
}
