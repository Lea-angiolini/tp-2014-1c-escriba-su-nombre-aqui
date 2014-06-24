#ifndef CPU_H_
#define CPU_H_

#define SINPROCESOACTIVO 0

#include "commons/sockets.h"

typedef struct {
	uint32_t cpuId;
	uint32_t pidProcesando;
	uint32_t socket;
} CPU;



void fnNuevoCpu	(int socketPtr);
uint32_t 	recibirYProcesarMensajesCpu	( CPU * cpu );
void borrarCPU( CPU * cpu );
void destruirTodasLasCPUS();
void removerPIDactivoACPU( uint32_t pidActivo);

#endif /* CPU_H_ */
