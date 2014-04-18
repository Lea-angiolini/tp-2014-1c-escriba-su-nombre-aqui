#include "colas.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "commons/collections/queue.h"

t_queue *newQueue, *readyQueue, *execQueue, *exitQueue, *blockQueue;
pthread_mutex_t readyQueueMutex = PTHREAD_MUTEX_INITIALIZER;

void crearColas()
{
	queue_create(newQueue);
	queue_create(readyQueue);
	queue_create(execQueue);
	queue_create(exitQueue);
	queue_create(blockQueue);
}

void destruirColas()
{
	queue_destroy(newQueue);
	queue_destroy(readyQueue);
	queue_destroy(execQueue);
	queue_destroy(exitQueue);
	queue_destroy(blockQueue);
}
