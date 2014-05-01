#ifndef KERNEL_H_
#define KERNEL_H_

#include "commons/sockets.h"

int escucharYEjecutarInstruccionesKernel();
int responder_orden_ejecucion( );
int procesarMenssajeKernel( int socket, socket_header header );
int orden_ejecucion() ;


#endif /* KERNEL_H_ */



