#ifndef KERNEL_H_
#define KERNEL_H_

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "memoria.h"
#include "Programa.h"

#include "commons/log.h"
#include "commons/sockets.h"


typedef struct {
	int socket;
} Kernel;


void  fnKernelConectado(int socket);
uint32_t procesarMenssajeKernel(Kernel * kernel,	socket_pedirMemoria * segmentosAreservar);
uint32_t recibirYProcesarMensajesKernel(Kernel * kernel);
uint32_t tamanioSegmentos(socket_pedirMemoria * segmentosAreservar);

#endif /* KERNEL_H_ */
