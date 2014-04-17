#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>
#include "sockets.h"

typedef struct {
	uint32_t id;
	uint32_t codeSegment;
	uint32_t stackSegment;
	uint32_t stackIndex;
	uint32_t etiquetaIndex;
	uint32_t programCounter;
	uint32_t contextSize;
	//Agregados
	int programaSocket;
	uint32_t prioridad;
} pcb_t;

typedef struct {
	socket_header header;
	pcb_t pcb;
} socket_pcb;

#endif /* PCB_H_ */
