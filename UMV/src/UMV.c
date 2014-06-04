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
#include <sys/socket.h>

#include "Consola.h"
#include "Kernel.h"
#include "CPU.h"

#include "mocks.h"

#include "commons/sockets.h"
#include "commons/log.h"
#include "commons/config.h"

#include "commons/collections/list.h"




int socketKernel;
t_log * logger;
t_list * cpus; //Lista en la que se van a guardar toda la info de cada cpu que se conecte
t_list * programas;
t_list * tabla_segmentos;

t_config * umvConfig;


pthread_t threadConsola;
pthread_t threadConexiones;
/*pthread_t threadKernel;
pthread_t threadCpus;
*/
/*
 *
 * ACA, en este puntero se va a guardar toda la memoria virtual
 *
 */
void * memoria;
uint32_t memoria_size;
uint32_t retardoUMV;
char * modoActualCreacionSegmentos;





void * handShake( void * socket ){
	socket_header * quienSos = malloc( sizeof( socket_header));
	if( sizeof(socket_header) == recv( *( int *)socket, quienSos, sizeof( socket_header), MSG_WAITALL)){

	switch( quienSos->code){

	case 'k': fnKernelConectado( (int *) socket);
			  break;

	case 'c': fnNuevoCpu( (int *) socket);
			  break;

	default: log_error( logger, "El codigo enviado por quien trataba de conectarse es invalido");
		}
	}else
		log_error(logger, "No se ha recibido con exito quien trataba de conectarse");
	return NULL;
}

void * crearConexiones(){

	while(1){
	crearServidor( config_get_int_value( umvConfig, "PUERTO"), handShake, logger);
	}
	return NULL;
}


//TODO enviar codigo de error si no se puede bindear el socket
/*void * iniciarServidorCpu() {

	crearServidor(config_get_int_value(umvConfig, "PUERTOCPU"), fnNuevoCpu, logger);
	return NULL;

}

//TODO enviar codigo de error si no se puede bindear el socket
void * iniciarServidorKernel() {
	crearServidor(config_get_int_value(umvConfig, "PUERTOKERNEL"), fnKernelConectado, logger);
	return NULL;
}
*/

int leerConfiguraciones() {
	umvConfig = config_create("config.cfg");

	if (!config_has_property(umvConfig, "PUERTO") || !config_has_property(umvConfig, "MEMORIA") || !config_has_property(umvConfig, "RETARDOUMV") || !config_has_property(umvConfig, "MODOCREACIONSEGMENTOS")) {
			log_error(logger, "Archivo de configuracion invalido");
			config_destroy(umvConfig);
			return -1;
	}
	/*if (!config_has_property(umvConfig, "PUERTOCPU")
			|| !config_has_property(umvConfig, "PUERTOKERNEL")
			|| !config_has_property(umvConfig, "MEMORIA") || !config_has_property(umvConfig, "MODOCREACIONSEGMENTOS") || !config_has_property(umvConfig, "RETARDOUMV")) {
		log_error(logger, "Archivo de configuracion invalido");
		config_destroy(umvConfig);
		return -1;
	}*/

	return 1;
}



int setUp() {

	leerConfiguraciones();

	cpus				= list_create();
	programas			= list_create();
	tabla_segmentos		= list_create();

	memoria_size = config_get_int_value(umvConfig, "MEMORIA");
	retardoUMV = config_get_int_value(umvConfig, "RETARDOUMV");
	modoActualCreacionSegmentos = config_get_string_value(umvConfig, "MODOCREACIONSEGMENTOS");

	log_info( logger, "Reservando %d Bytes de memoria", memoria_size );
	memoria = malloc( memoria_size );
	if (memoria == 0) {
		log_error(logger, "No se pudo alocar la memoria, finalizando...");
		return -1;
	}
	return 1;

}


int startThreads() {
	pthread_create(&threadConsola, NULL, iniciarConsola, NULL);
	/*pthread_create(&threadKernel, NULL, iniciarServidorKernel, NULL);
	pthread_create(&threadCpus, NULL, iniciarServidorCpu, NULL);*/
	pthread_create( &threadConexiones, NULL, crearConexiones, NULL);

	if ( pthread_join(threadConsola, NULL) || pthread_join(threadConexiones, NULL)) {
			log_error(logger, "Hubo un error esperando a algun hilo");
			return -1;
	}

	/*if ( pthread_join(threadConsola, NULL) || pthread_join(threadKernel, NULL) || pthread_join(threadCpus, NULL) ) {
		log_error(logger, "Hubo un error esperando a algun hilo");
		return -1;
	}*/
	return 1;
}


int main( int argc, char * argv[]) {

	if (argc != 2) {
		printf("Modo de empleo: ./UMV config.cfg\n");
		return EXIT_SUCCESS;
	}

	logger = log_create("log.txt", "UMV", 1, LOG_LEVEL_TRACE);

	log_info(logger, "Iniciando UMV...");

	setUp();
	//ejecutar();
	startThreads();


	free(memoria);
	config_destroy(umvConfig);
	log_destroy(logger);

	return EXIT_SUCCESS;


}

























