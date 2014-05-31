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
	uint32_t pid;
} cpu_info_t;

void *IniciarPcp(void *arg);
void *Dispatcher(void *arg);

bool iniciarServidorCpu();

void MoverReadyAExec();

void bajarNivelMultiprogramacion();

bool conexionCPU(int socket);
void desconexionCPU(int socket);
bool recibirYprocesarPedido(int socket);

bool terminoQuantumCPU(int socket);
void mensajeYDesconexionPrograma(int programaSocket, char *mensaje);

#endif /* PCP_H_ */
