#include <unistd.h>

#include "umv.h"
#include "kernel.h"
#include "stack.h"

#include "commons/log.h"
#include "commons/pcb.h"
#include "commons/parser/parser.h"


extern AnSISOP_funciones * ansisop_funciones;
extern AnSISOP_kernel * ansisop_Kernelfunciones;
extern t_log * logger;
extern pcb_t PCB_enEjecucion;
extern uint32_t quantumRestante;
extern uint32_t retardoQuantum;

//TODO no esta hecho todavia, es semi pseudocodigo
bool ejecutar () {


	log_info( logger, "Inicio la ejecucion del programa %d", PCB_enEjecucion.id );

	log_trace( logger, "Solicito el stack a la UMV");
	if( !obtenerContextStack() ) {
		return false;
	}

	while( quantumRestante > 0 ) //Agregar condicion para salida por excepcion o llamada bloqueante
	{

		char * instruccion = solicitarLineaPrograma();

		if( instruccion == -1 ){
			log_error( logger, "No se pudo obtener la linea" );
			return false;
		}

		log_trace( logger, "Incrementando el program counter" );
		PCB_enEjecucion.programCounter++;
		log_info( logger, "Ejecutando la linea obtenida: %s", instruccion );
		analizadorLinea( instruccion, ansisop_funciones, ansisop_Kernelfunciones );
		free( instruccion );
		log_debug( logger, "Finalizo la linea" );
		quantumRestante--;
		usleep(retardoQuantum*1000);

	}

	if( quantumRestante == 0 ) {
		log_info( logger, "Finalizo el quantum");
	}

	//log_info( logger, "Enviando el stack a la UMV");
	guardarStack();
	enviarFinQuantum( PCB_enEjecucion.id );

	return true;

}




