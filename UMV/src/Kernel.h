#ifndef KERNEL_H_
#define KERNEL_H_

#include "commons/sockets.h"

typedef struct {

	int socket;

} Kernel;



void  fnKernelConectado(int * socketPtr);
int procesarMenssajeKernel(Kernel * kernel,	socket_pedirMemoria * segmentosAreservar);
int recibirYProcesarMensajesKernel(Kernel * kernel);
int tamanioSegmentos(socket_pedirMemoria * segmentosAreservar);

#endif /* KERNEL_H_ */
