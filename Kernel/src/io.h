#ifndef IO_H_
#define IO_H_

#include <pthread.h>
#include "commons/collections/list.h"
#include "commons/collections/queue.h"



 struct io{
	char* nombre;
	int retardo;
	pthread_t thread;
	t_queue *cola;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
};

typedef struct io io_t;

t_list *armar_lista_dispositivos(char** hioId, char** hio);
void *hilo_io(void *ptr);
io_t *crear_registro(char* hioId, char* hioRetardo);

#endif /* IO_H_ */
