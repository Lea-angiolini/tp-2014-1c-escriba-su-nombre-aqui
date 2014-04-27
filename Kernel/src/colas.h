#ifndef COLAS_H_
#define COLAS_H_

#include <pthread.h>
#include "commons/collections/queue.h"

extern t_queue *newQueue; //La usa el PLP
extern t_queue *readyQueue; //La usa el PLP y PCP (hay que protegerla)
extern t_queue *execQueue; //La usa el PCP
extern t_queue *exitQueue; //La usa el PCP
extern t_queue *blockQueue; //Las usa el PCP y el IO?

extern pthread_mutex_t newQueueMutex; //Hay que protegerla porque la usan dos hilos del PLP
extern pthread_mutex_t readyQueueMutex;

void crearColas();
void destruirColas();

#endif /* COLAS_H_ */
