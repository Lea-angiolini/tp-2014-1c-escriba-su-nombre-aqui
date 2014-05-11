#ifndef CPU_H_
#define CPU_H_


#include "commons/sockets.h"

typedef struct {

	int cpuId;
	int pidProcesando;
	int socket;

} CPU;



int		procesarSolicitudDeLinea	( CPU * cpu, socket_obtenerLineaCodigo * solicitud );
void *	fnNuevoCpu					( void * socketPtr );
int 	recibirYProcesarMensajesCpu	( CPU * cpu );



#endif /* CPU_H_ */
