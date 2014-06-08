#include <stdio.h>
#include <stdlib.h>

#include "plp.h"
#include "pcp.h"
#include "io.h"
#include "colas.h"
#include "config.h"

extern t_log *logplp, *logpcp;
sem_t semKernel;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Kernel config.cfg\n");
		return EXIT_SUCCESS;
	}

	if( !cargar_config(argv[1]) ) {
		printf("Archivo de configuracion invalido\n");
		return EXIT_SUCCESS;
	}

	crear_colas();


	sem_init(&semKernel, 0, 0);

	pthread_t plpThread, pcpThread;

	if( pthread_create(&plpThread, NULL, &IniciarPlp, NULL) != 0 )
	{
		log_error(logplp, "Error al iniciar el hilo de PLP");
		sem_post(&semKernel);
	}

	if( pthread_create(&pcpThread, NULL, &IniciarPcp, NULL) != 0 )
	{
		log_error(logpcp, "Error al iniciar el hilo de PCP");
		sem_post(&semKernel);
	}

	sem_wait(&semKernel);

	pthread_cancel(plpThread);
	pthread_cancel(pcpThread);

	sem_destroy(&semKernel);

	destruir_colas();
	destruir_config();

	return EXIT_SUCCESS;
}
