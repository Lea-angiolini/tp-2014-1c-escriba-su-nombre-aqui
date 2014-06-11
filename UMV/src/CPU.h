#ifndef CPU_H_
#define CPU_H_


#include "commons/sockets.h"

typedef struct {

	uint32_t cpuId;
	uint32_t pidProcesando;
	uint32_t socket;

} CPU;



void fnNuevoCpu	( uint32_t * socketPtr );
uint32_t 	recibirYProcesarMensajesCpu	( CPU * cpu );
void borrarCPU( CPU * cpu );
void destruirTodasLasCPUS();

#endif /* CPU_H_ */
