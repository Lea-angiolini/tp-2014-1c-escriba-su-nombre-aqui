#ifndef SYSTEMCALLS_H_
#define SYSTEMCALLS_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <pthread.h>

#include "commons/sockets.h"
#include "commons/pcb.h"
#include "commons/log.h"
#include "commons/collections/dictionary.h"

bool syscallIO(int socket);
bool syscallObtenerValor(int socket);
bool syscallGrabarValor(int socket);
bool syscallWait(int socket);
bool syscallSignal(int socket);
bool syscallImprimirTexto(int socket);




#endif /* SYSTEMCALLS_H_ */
