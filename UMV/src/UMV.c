/*
 ============================================================================
 Name        : UMV.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#include "Consola.h"
#include "Kernel.h"
#include "CPU.h"

#include "commons/sockets.h"
#include "commons/log.h"
#include "commons/config.h"

#include "commons/collections/list.h"




int socketKernel;
t_list * cpus; //Lista en la que se van a guardar toda la info de cada cpu que se conecte
t_log * logger;




//TODO enviar codigo de error si no se puede bindear el socket
void * iniciarServidorCpu( void * config ){
	//crearServidor(int puerto, void* (*fn_nuevo_cliente)( void * socket ), t_log * log);
	t_config * umvConfig = (t_config*) config;

	crearServidor( config_get_int_value( umvConfig, "PUERTOCPU" ) , fnNuevoCpu, logger );

	return NULL;
}


//TODO enviar codigo de error si no se puede bindear el socket
void * iniciarServidorKernel( void * config ){
	t_config * umvConfig = (t_config*) config;
	crearServidor( config_get_int_value( umvConfig, "PUERTOKERNEL" ) , fnKernelConectado, logger );
	return NULL;
}






int main(void) {


	logger					= log_create( "log.txt", "UMV", 1, LOG_LEVEL_TRACE );
	t_config * umvConfig	= config_create( "resources/config.cfg" );
	cpus					= list_create();


	log_info( logger, "Iniciando UMV..." );



	if( ! config_has_property( umvConfig, "PUERTOCPU" ) ||  ! config_has_property( umvConfig, "PUERTOKERNEL" ) || ! config_has_property( umvConfig, "MEMORIA" ) ){
		log_error( logger, "Archivo de configuracion invalido" );
		return -1;
	}


	pthread_t threadConsola;
	pthread_create ( &threadConsola, NULL, iniciarConsola,			(void*) NULL );

	pthread_t threadKernel;
	pthread_create ( &threadKernel, NULL, iniciarServidorKernel, 	(void*) umvConfig );

	pthread_t threadCpu;
	pthread_create ( &threadCpu, NULL, 	iniciarServidorCpu, 		(void*) umvConfig );


	if( pthread_join( threadConsola, NULL ) || pthread_join( threadKernel, NULL ) || pthread_join( threadCpu, NULL ) ) {

		log_error( logger, "Hubo un error con los hilo" );
		return 2;

	}




	config_destroy	( umvConfig );
	log_destroy		( logger );

	return EXIT_SUCCESS;


}

























