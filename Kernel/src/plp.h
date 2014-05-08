#ifndef PLP_H_
#define PLP_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/sockets.h"
#include "commons/pcb.h"
#include "commons/parser/metadata_program.h"

void *IniciarPlp(void *arg);

void MoverNewAReady();
void puedoMoverNewAReady();

bool nuevoMensaje(int socket);
void desconexionCliente();
bool recibirYprocesarScript(int socket);

#endif /* PLP_H_ */
