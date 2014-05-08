#include "colas.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "commons/collections/queue.h"

t_queue *newQueue, *readyQueue, *execQueue, *exitQueue, *blockQueue;
pthread_mutex_t readyQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t blockQueueMutex = PTHREAD_MUTEX_INITIALIZER;

t_queue *cpuReadyQueue, *cpuExecQueue;

void crear_colas()
{
	newQueue = queue_create();
	readyQueue = queue_create();
	execQueue = queue_create();
	exitQueue = queue_create();
	blockQueue = queue_create();

	cpuReadyQueue = queue_create();
	cpuExecQueue = queue_create();
}

void destruir_colas()
{
	queue_destroy_and_destroy_elements(newQueue, free);
	queue_destroy_and_destroy_elements(readyQueue, free);
	queue_destroy_and_destroy_elements(execQueue, free);
	queue_destroy_and_destroy_elements(exitQueue, free);
	queue_destroy_and_destroy_elements(blockQueue, free);

	queue_destroy_and_destroy_elements(cpuReadyQueue, free);
	queue_destroy_and_destroy_elements(cpuExecQueue, free);
}
