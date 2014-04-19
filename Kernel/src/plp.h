#ifndef PLP_H_
#define PLP_H_

#include <stdbool.h>
#include <pthread.h>
#include "commons/sockets.h"
#include "commons/pcb.h"

void *IniciarPlp(void *arg);
void MoverNewAReady();
bool sjnAlgorithm(pcb_t *a, pcb_t *b);
void puedoMoverNewAReady();
bool nuevoMensaje(int socket);
void desconexionCliente();
bool recibirYprocesarScript(int socket, socket_header header);

#endif /* PLP_H_ */
