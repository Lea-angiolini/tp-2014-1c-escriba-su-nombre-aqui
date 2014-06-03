#ifndef PLP_H_
#define PLP_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/sockets.h"
#include "commons/pcb.h"
#include "commons/parser/metadata_program.h"

void *IniciarPlp(void *arg);
bool iniciarServidorProgramas();
bool conectarUMV();

void MoverNewAReady();
void puedoMoverNewAReady();

void desconexionCliente();
bool recibirYprocesarScript(int socketPrograma);

#endif /* PLP_H_ */
