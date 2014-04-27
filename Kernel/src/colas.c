#include "colas.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "commons/collections/queue.h"

t_queue *newQueue, *readyQueue, *execQueue, *exitQueue, *blockQueue;
pthread_mutex_t readyQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t newQueueMutex = PTHREAD_MUTEX_INITIALIZER;

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
	queue_destroy_and_destroy_elements(newQueue, free);
	queue_destroy_and_destroy_elements(readyQueue, free);
	queue_destroy_and_destroy_elements(execQueue, free);
	queue_destroy_and_destroy_elements(exitQueue, free);
	queue_destroy_and_destroy_elements(blockQueue, free);
}
