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


void ejecutar () {

	log_info(logger, "Inicio la ejecucion del programa %d", PCB_enEjecucion.id);

	if( !generarDiccionarioVariables() || !obtenerEtiquetas() ) {
		PCB_enEjecucion.lastErrorCode = 4;
		//No hago un return false porque la cpu respondio bien, pero se debe informar al kernel, si hiciera return false ni llegaria al Kernel
		return;
	}

	while( quantumRestante > 0 && PCB_enEjecucion.lastErrorCode == 0 ) //Agregar condicion para salida por excepcion o llamada bloqueante
	{

		char * instruccion = solicitarLineaPrograma();

		if( instruccion == -1 ){
			log_error( logger, "No se pudo obtener la linea" );
			return;
		}

		log_debug( logger, "Incrementando el program counter" );
		PCB_enEjecucion.programCounter++;
		quantumRestante--;
		log_info( logger, "Ejecutando la linea obtenida: %s", instruccion );
		analizadorLinea( instruccion, ansisop_funciones, ansisop_Kernelfunciones );
		free(instruccion);
		log_debug( logger, "Finalizo la linea" );
		usleep(retardoQuantum*1000);

	}

	if( quantumRestante == 0 ) {
		log_info(logger, "Finalizo el quantum");
	}

	if(! enviarFinQuantum(PCB_enEjecucion.id)){
		log_error( logger, "Hubo un problema al sincronizar los datos con la UMV" );
		PCB_enEjecucion.lastErrorCode = 4;
	}

	return;
}




