#ifndef PCP_H_
#define PCP_H_

#include <stdbool.h>
#include "commons/sockets.h"

void *IniciarPcp(void *arg);

void desconexionCPU();
bool nuevoMensajeCPU(int socket);
bool recibirYprocesarPedido(int socket, socket_header header);


#endif /* PCP_H_ */
