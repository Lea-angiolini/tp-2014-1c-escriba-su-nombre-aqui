#include "commons/pcb.h"
#include "umv.h"
#include "commons/parser/parser.h"


extern AnSISOP_funciones * ansisop_funciones;
extern t_log * logger;
extern pcb_t * PCB_enEjecucion;

//TODO no esta hecho todavia, es semi pseudocodigo
pcb_t * ejecutar () {


	enviarCambioContexto( PCB_enEjecucion->id );


	//while( 1 ){

		log_debug( logger, "Solicitando linea del programa a la umv" );
		char * instruccion = solicitarLineaPrograma( PCB_enEjecucion->programCounter );
		log_debug( logger, "Incrementando el program counter" );
		PCB_enEjecucion->programCounter++;
		log_debug( logger, "Ejecutando la intruccion !! " );
		analizadorLinea( instruccion, ansisop_funciones, NULL );

	//}


	enviarFinQuantum( PCB_enEjecucion->id );

	return PCB_enEjecucion;

}




