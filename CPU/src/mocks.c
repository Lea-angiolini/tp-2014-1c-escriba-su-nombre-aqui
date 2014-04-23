#include "ejecucion.h"

extern pcb_t * PCB_enEjecucion;

void ejecutarPrueba(){

	PCB_enEjecucion->codeSegment = 45;
	PCB_enEjecucion->contextSize = 45;
	PCB_enEjecucion->etiquetaIndex = 2;
	PCB_enEjecucion->id = 4;
	PCB_enEjecucion->lastErrorCode = 0;
	PCB_enEjecucion->prioridad = 2;
	PCB_enEjecucion->programCounter = 432;
	PCB_enEjecucion->programaSocket = 32423;
	//PCB_enEjecucion->stackIndex = 234;

	ejecutar();

}
