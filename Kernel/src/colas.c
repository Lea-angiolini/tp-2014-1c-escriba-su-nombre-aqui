#include "colas.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "commons/collections/queue.h"

t_queue *newQueue, *readyQueue, *execQueue, *exitQueue, *blockQueue;
pthread_mutex_t readyQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t blockQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t execQueueMutex = PTHREAD_MUTEX_INITIALIZER;

t_queue *cpuReadyQueue, *cpuExecQueue;

pthread_mutex_t cpuReadyQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cpuExecQueueMutex = PTHREAD_MUTEX_INITIALIZER;

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

pcb_t *list_remove_pcb_by_pid(t_list *self, uint32_t pid)
{
	bool matchearPCB(pcb_t *pcb) {
		return pcb->id == pid;
	}
	return list_remove_by_condition(self, matchearPCB);
}
