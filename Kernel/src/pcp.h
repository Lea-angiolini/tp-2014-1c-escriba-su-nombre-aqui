#ifndef PCP_H_
#define PCP_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

#include "commons/sockets.h"
#include "commons/pcb.h"
#include "commons/log.h"
#include "commons/config.h"

void *IniciarPcp(void *arg);
void *Dispatcher(void *arg);

void bajarNivelMultiprogramacion();

void conexionCPU(int socket);
void desconexionCPU(int socket);
bool nuevoMensajeCPU(int socket);
bool recibirYprocesarPedido(int socket);

typedef struct {
	int socketCPU;
	int socketPrograma;
} cpu_info_t;

#endif /* PCP_H_ */
