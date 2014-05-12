#ifndef COLAS_H_
#define COLAS_H_

#include <pthread.h>
#include "commons/collections/queue.h"

extern t_queue *newQueue; //La usa el PLP
extern t_queue *readyQueue; //La usa el PLP, PCP y el IO
extern t_queue *execQueue; //La usa el PCP y el Dispatcher
extern t_queue *exitQueue; //La usa el PCP
extern t_queue *blockQueue; //Las usa el PCP y el IO

extern pthread_mutex_t readyQueueMutex;
extern pthread_mutex_t blockQueueMutex;
extern pthread_mutex_t execQueueMutex;


extern t_queue *cpuReadyQueue;
extern t_queue *cpuExecQueue;

extern pthread_mutex_t cpuReadyQueueMutex;
extern pthread_mutex_t cpuExecQueueMutex;

void crear_colas();
void destruir_colas();

#endif /* COLAS_H_ */
