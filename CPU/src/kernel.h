#ifndef KERNEL_H_
#define KERNEL_H_



int escucharYEjecutarInstruccionesKernel();
int responder_orden_ejecucion( int socket );
int procesarMenssajeKernel( int socket, socket_header header );
int orden_ejecucion( int socket, socket_header header ) ;


#endif /* KERNEL_H_ */



