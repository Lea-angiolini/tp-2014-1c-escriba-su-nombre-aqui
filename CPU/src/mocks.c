#include "ejecucion.h"
#include "umv.h"
#include "commons/log.h"

extern pcb_t * PCB_enEjecucion;
extern t_log * logger;

int ejecutarPrueba() {

	PCB_enEjecucion->codeSegment = 45;
	PCB_enEjecucion->contextSize = 45;
	PCB_enEjecucion->etiquetaIndex = 2;
	PCB_enEjecucion->id = 4;
	PCB_enEjecucion->lastErrorCode = 0;
	PCB_enEjecucion->prioridad = 2;
	PCB_enEjecucion->programCounter = 432;
	PCB_enEjecucion->programaSocket = 32423;
	//PCB_enEjecucion->stackIndex = 234;

	int i = 0;
	for( i = 0; i < 4; i++ ){
		solicitarLineaPrograma( 45 );
	}


	//ejecutar();
	return 1;

}
