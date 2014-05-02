#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "io.h"
#include "colas.h"
#include "commons/pcb.h"

struct data_cola{
	uint32_t pid;
	uint32_t tiempo;
};
typedef struct data_cola data_cola_t;

void *hilo_io(void *ptr){
	io_t *parametros = (io_t *) ptr;
	pthread_mutex_lock(&parametros->mutex);

	while(1){

	pthread_cond_wait(&parametros->condition,&parametros->mutex);

	//saco el primer elemento de la cola del dispositivo para activarlo
	data_cola_t *orden_activa = queue_pop(parametros->cola);

	//aplico el retardo
	usleep((orden_activa->tiempo) * (parametros->retardo));

	//funcion usada como condicion para buscar el pcb correspondiente en la blockQueue
	bool matchearPCB (pcb_t *pcb){
		return (pcb->id == orden_activa->pid);
	}

	//Saco el pcb de la cola de bloqueados y la pongo en la cola de ready
	pcb_t *pcb_blocked_to_ready = list_remove_by_condition(blockQueue->elements,matchearPCB);

	queue_push(readyQueue,pcb_blocked_to_ready);



	}
	pthread_mutex_unlock(&parametros->mutex);

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

	//if(sizeof(hioId)){
	//	int i = 0;

	//	for(;i<(sizeof(hioId)-1);i++){
	//		io_t *registro_hio = crear_registro(hioId[i], hioRetardo[i]);
	//		list_add(lista_dispositivos,registro_hio);
	//	}


	//}

	int i ;

	for(i=0;hioId[i] != NULL;i++){
		io_t *registro_hio = crear_registro(hioId[i], hioRetardo[i]);
		list_add(lista_dispositivos,registro_hio);
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

	return nuevo_registro;
}
