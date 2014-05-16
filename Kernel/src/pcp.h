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

typedef struct {
	int socketCPU;
	int socketPrograma;
} cpu_info_t;

void *IniciarPcp(void *arg);
void *Dispatcher(void *arg);

void bajarNivelMultiprogramacion();

bool conexionCPU(int socket);
void desconexionCPU(int socket);
bool nuevoMensajeCPU(int socket);
bool recibirYprocesarPedido(int socket);
bool syscallIO(int socket);
bool syscallObtenerValor(int socket);

#endif /* PCP_H_ */
