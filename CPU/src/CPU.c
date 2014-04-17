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
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>


int conexionKernel;
int conexionUMV;

int quantumRestante;


int main(void) {


	t_log * logger =	log_create( "log.txt", "CPU", 1, LOG_LEVEL_TRACE );

	if( crearConexiones( logger ) < 0 ){
		return -1;
	}

	log_info(logger, "El programa finalizo con exito !");
	return 0;

}


int crearConexiones(t_log * logger) {

	t_config * cpuConfig = config_create("resources/config.cfg");

	if ( !config_has_property(cpuConfig, "IPKERNEL")
			|| !config_has_property(cpuConfig, "PUERTOKERNEL")
			|| !config_has_property(cpuConfig, "IPUMV")
			|| !config_has_property(cpuConfig, "PUERTOUMV")) {

		log_error(logger, "Archivo de configuracion no valido");
		return -1;
	}

	conexionKernel = conectar(config_get_string_value(cpuConfig, "IPKERNEL"), config_get_int_value(cpuConfig, "PUERTOKERNEL"), logger);
	if (conexionKernel > 0) {
		log_error(logger, "No se pudo conectar al Kernel");
		return -1;
	}

	conexionUMV = conectar(config_get_string_value(cpuConfig, "IPUMV"), config_get_int_value(cpuConfig, "PUERTOUMV"), logger);
	if (conexionUMV > 0) {
		log_error(logger, "No se pudo conectar al UMV");
		return -1;
	}

	config_destroy(cpuConfig);

	return 1;
}


int cerrarConexiones( logger ){
	return 0;
}

int esperarMensajes(){


	return -1;
}




































