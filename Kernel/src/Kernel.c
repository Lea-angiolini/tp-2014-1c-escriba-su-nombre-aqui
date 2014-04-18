#include <stdio.h>
#include <stdlib.h>

#include "plp.h"
#include "pcp.h"
#include "io.h"
#include "colas.h"

#include "commons/config.h"

#include <stdint.h>

t_config *config;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Kernel config.cfg\n");
		return EXIT_SUCCESS;
	}

	config = config_create(argv[1]);
	crearColas();

	pthread_t plpThread, pcpThread;

	pthread_create(&plpThread, NULL, &IniciarPlp, NULL);
	pthread_create(&pcpThread, NULL, &IniciarPcp, NULL);

	pthread_join(plpThread, NULL);
	pthread_join(pcpThread, NULL);

	//config_get_int_value(config, "PUERTO_CPU");
	//config_get_int_value(config, "QUANTUM");
	//config_get_int_value(config, "RETARDO");
	//char** semaforos = config_get_array_value(config, "SEMAFOROS");
	//char** valor_semaforos = config_get_array_value(config, "VALOR_SEMAFORO");
	//char** hio = config_get_array_value(config, "HIO");
	//char** hioId = config_get_array_value(config, "ID_HIO");
	//char** variablesCompartidas = config_get_array_value(config, "VARIABLES_COMPARTIDAS");

	destruirColas();
	config_destroy(config);

	return EXIT_SUCCESS;
}
