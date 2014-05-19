#ifndef KERNEL_H_
#define KERNEL_H_

#include "commons/sockets.h"


uint32_t	solcitarVariableCompartidaAKernel(t_nombre_compartida variable);
int			enviarAKernelNuevoValorVariableCompartida(t_nombre_compartida variable, t_valor_variable valor);
int			enviarAKernelImprimir		( t_valor_variable valor );
int			enviarAKernelImprimirTexto	( char * texto );
int			enviarAKernelEntradaSalida	(t_nombre_dispositivo dispositivo, int tiempo);

int escucharYEjecutarInstruccionesKernel();
int responder_orden_ejecucion( );
int procesarMenssajeKernel( int socket, socket_header header );
int orden_ejecucion() ;

int enviarPCB();

#endif /* KERNEL_H_ */



