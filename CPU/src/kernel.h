#ifndef KERNEL_H_
#define KERNEL_H_

#include "commons/sockets.h"


uint32_t	solcitarVariableCompartidaAKernel(t_nombre_compartida variable);
bool		enviarAKernelNuevoValorVariableCompartida(t_nombre_compartida variable, t_valor_variable valor);
int			enviarAKernelImprimir		( t_valor_variable valor );
int			enviarAKernelImprimirTexto	( char * texto );
bool		enviarAKernelEntradaSalida	(t_nombre_dispositivo dispositivo, int tiempo);
bool 		enviarAKernelSignal(t_nombre_semaforo identificador_semaforo);
bool 		enviarAKernelWait(t_nombre_semaforo identificador_semaforo);

int escucharYEjecutarInstruccionesKernel();
int responder_orden_ejecucion( );
int procesarMenssajeKernel( int socket, socket_header header );
int orden_ejecucion() ;

int enviarHeaderCodeaKernel(unsigned char code);
int enviarPCB();

#endif /* KERNEL_H_ */



