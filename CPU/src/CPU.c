#include <stdio.h>
#include <stdbool.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/pcb.h"

#include "config.h"
#include "umv.h"
#include "kernel.h"

#include "mocks.h"

t_log * logger;

uint32_t quantumPorEjecucion;
uint32_t retardoQuantum;
uint32_t quantumRestante;

pcb_t PCB_enEjecucion;


int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./CPU config.cfg\n");
		return EXIT_SUCCESS;
	}

	char *logName = string_from_format("log_%d.txt", process_getpid());
	logger = log_create("log.txt", "CPU", 1, LOG_LEVEL_TRACE);

	if( !cargar_config(argv[1]) ) {
		printf("Archivo de configuracion invalido\n");
		return EXIT_SUCCESS;
	}

	log_info(logger, "Iniciando CPU (log: %s)", logName);

	if ( !crearConexionKernel() || !crearConexionUMV() ) {
		log_error(logger, "Hubo un error en el proceso CPU, finalizando");
		return EXIT_SUCCESS;
	}


	//ejecutarPrueba();
	if( recibirYProcesarMensajesKernel() ){
		log_info(logger, "El programa finalizo con correctamente");
	}else{
		log_error( logger, "Hubo un problema y se finaliza la CPU" );
	}


	log_destroy(logger);

	destruir_config();

	return EXIT_SUCCESS;

}

