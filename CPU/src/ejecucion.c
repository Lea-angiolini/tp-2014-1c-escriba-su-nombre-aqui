#include "umv.h"

#include "commons/log.h"

#include "commons/pcb.h"
#include "commons/parser/parser.h"


extern AnSISOP_funciones * ansisop_funciones;
extern t_log * logger;
extern pcb_t PCB_enEjecucion;
extern uint32_t quantumRestante;

//TODO no esta hecho todavia, es semi pseudocodigo
pcb_t ejecutar () {


	while( quantumRestante > 0 ) {

		log_debug( logger, "Solicitando linea del programa a la umv" );
		char * instruccion = solicitarLineaPrograma( PCB_enEjecucion.programCounter );
		log_debug( logger, "Incrementando el program counter" );
		PCB_enEjecucion.programCounter++;
		log_debug( logger, "Ejecutando la intruccion !! " );
		analizadorLinea( instruccion, ansisop_funciones, NULL );

	}


	//sincronizarStack();
	enviarFinQuantum( PCB_enEjecucion.id );

	return PCB_enEjecucion;

}




