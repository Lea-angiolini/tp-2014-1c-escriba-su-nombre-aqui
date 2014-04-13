#include <stdio.h>
#include <stdlib.h>

#include "commons/config.h"

typedef struct {
	unsigned int id;
	unsigned int codeSegment;
	unsigned int stackSegment;
	unsigned int stackIndex;
	unsigned int etiquetaIndex;
	unsigned int programCounter;
	unsigned int contextSize;
	//int ProgramaSocket;
} PCB;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Kernel config.cfg\n");
		return EXIT_SUCCESS;
	}

	t_config *config = config_create(argv[1]);

	//config_get_int_value(config, "PUERTO_PROG");
	//config_get_int_value(config, "PUERTO_CPU");
	//config_get_int_value(config, "QUANTUM");
	//config_get_int_value(config, "RETARDO");
	//config_get_int_value(config, "MULTIPROGRAMACION");
	//char** semaforos = config_get_array_value(config, "SEMAFOROS");
	//char** valor_semaforos = config_get_array_value(config, "VALOR_SEMAFORO");
	//char** hio = config_get_array_value(config, "HIO");
	//char** hioId = config_get_array_value(config, "ID_HIO");
	//char** variablesCompartidas = config_get_array_value(config, "VARIABLES_COMPARTIDAS");

	config_destroy(config);

	return EXIT_SUCCESS;
}
