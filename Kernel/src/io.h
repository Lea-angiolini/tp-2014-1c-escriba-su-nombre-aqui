#ifndef IO_H_
#define IO_H_

#include <pthread.h>
#include "commons/config.h"
#include "commons/collections/list.h"
#include "commons/collections/queue.h"

 typedef struct {
	char* nombre;
	int retardo;
	pthread_t thread;
	t_queue *cola;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
} io_t;

typedef struct {
	uint32_t pid;
	uint32_t tiempo;
} data_cola_t;

void *hilo_io(void *ptr);
io_t *crear_registro(char* hioId, char* hioRetardo);

#endif /* IO_H_ */
