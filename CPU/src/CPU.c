/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "commons/sockets.h"
#include "commons/log.h"
#include "commons/config.h"
#include "commons/parser/parser.h"
#include "commons/pcb.h"
#include "ejecucion.h"
#include "mocks.h"

#include "primitivas.h"

#include "kernel.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>


int conexionKernel;
int conexionUMV;

int quantumRestante;
t_log * logger;

AnSISOP_funciones * ansisop_funciones;

pcb_t * PCB_enEjecucion;

t_config * cpuConfig;



int leerConfig(){
	cpuConfig = config_create("resources/config.cfg");

	if ( !config_has_property(cpuConfig, "IPKERNEL")
			|| !config_has_property(cpuConfig, "PUERTOKERNEL")
			|| !config_has_property(cpuConfig, "IPUMV")
			|| !config_has_property(cpuConfig, "PUERTOUMV")) {

		log_error(logger, "Archivo de configuracion no valido");
		return -1;
	}

	return 1;
}




int crearConexiones() {

	conexionKernel = conectar(config_get_string_value(cpuConfig, "IPKERNEL"), config_get_int_value(cpuConfig, "PUERTOKERNEL"), logger);
	if (conexionKernel < 0) {
		log_error(logger, "No se pudo conectar al Kernel");
		return -1;
	}

	conexionUMV = conectar(config_get_string_value(cpuConfig, "IPUMV"), config_get_int_value(cpuConfig, "PUERTOUMV"), logger);
	if (conexionUMV < 0) {
		close(conexionKernel);
		log_error(logger, "No se pudo conectar al UMV");
		return -1;
	}


	return 1;
}




int main(void) {


	logger = log_create( "log.txt", "CPU", 1, LOG_LEVEL_TRACE );

	//TODO verificar errores
	ansisop_funciones = crearAnSISOP_funciones();
	PCB_enEjecucion = malloc( sizeof( pcb_t ) );

	/*
	if( leerConfig() < 0 || crearConexiones() < 0 || recibirYProcesarMensajesKernel() < 0 ) {
		log_error( logger, "Hubo un error en el programa, finalizando :( " );
		return -1;
	}else{
		log_info( logger, "El programa finalizo con exito !" );
	}

	log_destroy( logger );
	config_destroy( cpuConfig );
	free( PCB_enEjecucion );
	free( ansisop_funciones );
	*/

	ejecutarPrueba();


	return 0;

}

































