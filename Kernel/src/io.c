#include <stdlib.h>
#include <stdio.h>

#include "io.h"

struct data_cola{
	uint32_t id_pcb;
	int tiempo;
};
typedef struct data_cola data_cola_t;

void *hilo_io(void *ptr){
	io_t *parametros = (io_t *) ptr;

	for(;;){
		pthread_mutex_lock(&parametros->mutex);
		if (queue_is_empty(parametros->cola))
			pthread_cond_wait(&parametros->condition,&parametros->mutex);

	//saco el primer elemento de la cola para activarlo
	data_cola_t *elemento_activo = queue_pop(parametros->cola);

	//hay que ver el tiempo ese, porque es en segundos!!!!
	sleep(elemento_activo->tiempo);

	//Saco el pcb de la cola de bloqueados y la pongo en la cola de ready
	pcb_t pcb_blocked_to_ready = list_find(BlockedQueue,condicion);
	list_remove(BlockedQueue, indexComoSeBusque);
	queue_push(ReadyQueue,pcb_blocked_to_ready);

	pthread_mutex_unlock(&parametros->mutex);

	}

	return(0);
}

char* id_dispositivo(io_t *registro){
	return registro->nombre;
}
int ret_dispositivo(io_t *registro){
	return registro->retardo;
}

t_list *armar_lista_dispositivos (char** hioId, char** hioRetardo){

	t_list *lista_dispositivos;
	lista_dispositivos = list_create();

	if(sizeof(hioId)){
		int i = 0;

		for(;i<sizeof(hioId);i++){
			io_t *registro_hio = crear_registro(hioId[i], hioRetardo[i]);
			list_add(lista_dispositivos,registro_hio);
		}


	}
	return (lista_dispositivos);
}


io_t *crear_registro(char* hioId, char* hioRetardo){

	//creo el registro del dispositivo io que va incluido en
	//la lista de dispositivos
	io_t *nuevo_registro = malloc(sizeof(io_t));

	nuevo_registro->nombre = hioId;
	nuevo_registro->cola = queue_create();
	nuevo_registro->retardo = atoi(hioRetardo);
	pthread_mutex_init(&nuevo_registro->mutex,NULL);
	pthread_cond_init(&nuevo_registro->condition,NULL);
	pthread_create(&(nuevo_registro->thread), NULL, &hilo_io,(void *)nuevo_registro);

	return (nuevo_registro);
}
