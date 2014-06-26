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


void  fnKernelConectado(int socketKernel);
bool recibirYProcesarPedidoKernel(int socketKernel);

bool crearSegmentos(int socketKernel);
bool borrarSegmentos(int socketKernel);
bool recibirSegmentos(int socketKernel, socket_pedirMemoria *pedidoMemoria);

#endif /* KERNEL_H_ */
