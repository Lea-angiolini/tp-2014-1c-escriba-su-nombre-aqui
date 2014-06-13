
#include "config.h"

#define WORSTFIT 1
#define FIRSTFIT 0

t_list * cpus; //Lista en la que se van a guardar toda la info de cada cpu que se conecte
t_list * programas;
t_list * tabla_segmentos;

t_config * umvConfig;

void * memoria;
uint32_t memoria_size;
uint32_t retardoUMV;
uint32_t modoActualCreacionSegmentos;

extern t_log * logger;

bool cargar_config(char *config){

	if (!leerConfiguraciones(config)){
		return false;

	}

	cpus				= list_create();
	programas			= list_create();
	tabla_segmentos		= list_create();

	memoria_size = config_get_int_value(umvConfig, "MEMORIA");
	retardoUMV = config_get_int_value(umvConfig, "RETARDOUMV");
	if( config_get_string_value(umvConfig, "MODOCREACIONSEGMENTOS") == "WORSTFIT"){
		modoActualCreacionSegmentos = WORSTFIT;
	}else {
		modoActualCreacionSegmentos = FIRSTFIT;
	}

	log_info( logger, "Reservando %d Bytes de memoria", memoria_size );
	memoria = malloc( memoria_size );
	if (memoria == 0) {
		log_error(logger, "No se pudo alocar la memoria, finalizando...");
		return false;
	}
	return true;


}

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

	crearServidor( config_get_int_value( umvConfig, "PUERTO"), handShake, logger);
	return NULL;
}

bool leerConfiguraciones( char * config) {
	umvConfig = config_create(config);

	if (!config_has_property(umvConfig, "PUERTO") || !config_has_property(umvConfig, "MEMORIA") || !config_has_property(umvConfig, "RETARDOUMV") || !config_has_property(umvConfig, "MODOCREACIONSEGMENTOS")) {
			log_error(logger, "Archivo de configuracion invalido");
			config_destroy(umvConfig);
			return false;
	}


	return true;
}

void destruir_config(){

	list_destroy_and_destroy_elements( tabla_segmentos, free );
	log_info(logger, "Todos los segmentos han sido liberados");
	list_destroy_and_destroy_elements( programas, free);
	log_info( logger, "Todos los programas han sido liberados");
	list_destroy_and_destroy_elements( cpus, free );
	log_info( logger, "Todas las CPUs han sido liberadas");

	free(memoria);
	config_destroy(umvConfig);

}
