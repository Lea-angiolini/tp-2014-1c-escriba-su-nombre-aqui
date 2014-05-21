#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include <stdbool.h>

#include "commons/sockets.h"
#include "commons/log.h"
#include "commons/config.h"
#include "commons/parser/parser.h"
#include "commons/pcb.h"

#include "stack.h"
#include "ejecucion.h"
#include "mocks.h"
#include "primitivas.h"
#include "kernel.h"

t_config * cpuConfig;
t_log * logger;

int conexionKernel;
int conexionUMV;

uint32_t quantumPorEjecucion;
uint32_t retardo;
uint32_t quantumRestante;

pcb_t PCB_enEjecucion;

AnSISOP_funciones * ansisop_funciones;

Stack * stackCache;

bool leerConfig() {
	cpuConfig = config_create("config.cfg");

	if (!config_has_property(cpuConfig, "IPKERNEL")
			|| !config_has_property(cpuConfig, "PUERTOKERNEL")
			|| !config_has_property(cpuConfig, "IPUMV")
			|| !config_has_property(cpuConfig, "PUERTOUMV"))
	{

		log_error(logger, "Archivo de configuracion no valido");
		return false;
	}

	return true;
}

int crearConexionKernel() {
	log_info(logger, "Conectando al Kernel en: %s : %d", config_get_string_value(cpuConfig, "IPKERNEL"), config_get_int_value(cpuConfig, "PUERTOKERNEL"));
	conexionKernel = conectar(config_get_string_value(cpuConfig, "IPKERNEL"), config_get_int_value(cpuConfig, "PUERTOKERNEL"), logger);

	if (conexionKernel < 0) {
		log_error(logger, "No se pudo conectar al Kernel");
		return -1;
	}

	return enviarHandshake();

}

bool crearConexionUMV() {
	log_info(logger, "Conectando a la UMV en %s : %d", config_get_string_value(cpuConfig, "IPUMV"), config_get_int_value(cpuConfig, "PUERTOUMV"));
	conexionUMV = conectar(config_get_string_value(cpuConfig, "IPUMV"),	config_get_int_value(cpuConfig, "PUERTOUMV"), logger);
	if (conexionUMV < 0) {
		close(conexionKernel);
		log_error(logger, "No se pudo conectar al UMV");
		return false;
	}
	return true;
}

bool crearConexiones() {

	if (crearConexionKernel() < 0 || crearConexionUMV() < 0) {
		return false;
	}
	return true;
}

int main(int argc, char *argv[]) {
	logger = log_create("log.txt", "CPU", 1, LOG_LEVEL_TRACE);
	log_info(logger, "Iniciando CPU");
	log_debug(logger, "Setando primitivas");
	ansisop_funciones = crearAnSISOP_funciones();
	stackCache = new_Stack();

	//if( leerConfig() < 0 || crearConexiones() < 0 || recibirYProcesarMensajesKernel() < 0 ) {
	if (!leerConfig() || !crearConexiones() || !ejecutarPrueba()) {
		log_error(logger, "Hubo un error en el proceso CPU, finalizando");
		return EXIT_SUCCESS;
	}

	log_info(logger, "El programa finalizo con correctamente");

	log_destroy(logger);
	config_destroy(cpuConfig);

	free(ansisop_funciones);
	free(stackCache);

	return EXIT_SUCCESS;

}

