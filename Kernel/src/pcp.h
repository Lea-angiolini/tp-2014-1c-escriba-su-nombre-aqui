#ifndef PCP_H_
#define PCP_H_

#include <stdbool.h>
#include "commons/sockets.h"
#include "commons/pcb.h"

void *IniciarPcp(void *arg);
void *Dispatcher(void *arg);

void bajarNivelMultiprogramacion();

void conexionCPU(int socket);
void desconexionCPU(int socket);
bool nuevoMensajeCPU(int socket);
bool recibirYprocesarPedido(int socket, socket_header header);

typedef struct {
	int socketCPU;
	int socketPrograma;
} cpu_info_t;

#endif /* PCP_H_ */
