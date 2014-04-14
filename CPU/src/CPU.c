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



int main(void) {


	t_log * log =	log_create( "log.txt", "CPU", 1, LOG_LEVEL_TRACE );
	t_config * cpuConfig = config_create( "resources/config.cfg" );


	if( !config_has_property( cpuConfig, "IPKERNEL" ) || !config_has_property( cpuConfig, "PUERTOKERNEL" ) || config_has_property( cpuConfig, "IPUMV" ) || config_has_property( cpuConfig, "PUERTOUMV" )){
		log_error(log, "Archivo de configuracion no valido");
		return -1;
	}


	int conexionKernel = conectar( config_get_string_value( cpuConfig, "IPKERNEL" ) , config_get_int_value( cpuConfig, "PUERTOKERNEL" ) , log );
	if(conexionKernel > 0){
		log_error(log, "No se pudo conectar al Kernel");
		return -1;
	}


	int conexionUMV	= conectar( config_get_string_value( cpuConfig, "IPUMV" ) , config_get_int_value( cpuConfig, "PUERTOUMV" ) , log );
	if(conexionUMV > 0){
		log_error(log, "No se pudo conectar al UMV");
		return -1;
	}

	config_destroy(cpuConfig);


	return 0;

}






