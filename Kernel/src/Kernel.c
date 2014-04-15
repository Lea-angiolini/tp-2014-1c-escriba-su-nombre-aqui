#include <stdio.h>
#include <stdlib.h>

#include "plp.h"
#include "pcp.h"
#include "io.h"

#include "commons/config.h"
#include "commons/collections/queue.h"

typedef struct {
	unsigned int id;
	unsigned int codeSegment;
	unsigned int stackSegment;
	unsigned int stackIndex;
	unsigned int etiquetaIndex;
	unsigned int programCounter;
	unsigned int contextSize;
	//Agregados
	int programaSocket;
	unsigned int prioridad;
} pcb_t;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Kernel config.cfg\n");
		return EXIT_SUCCESS;
	}

	t_config *config = config_create(argv[1]);
	//t_list *readyQueue;
	//t_queue *newQueue, *exitQueue, *execQueue, *blockQueue;

	pthread_t plpThread, pcpThread;

	pthread_create(&plpThread, NULL, &IniciarPlp, NULL);
	pthread_create(&pcpThread, NULL, &IniciarPcp, NULL);

	pthread_join(plpThread);
	pthread_join(pcpThread);

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
