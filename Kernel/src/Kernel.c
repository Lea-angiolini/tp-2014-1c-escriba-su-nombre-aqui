#include <stdio.h>
#include <stdlib.h>

#include "plp.h"
#include "pcp.h"
#include "io.h"
#include "colas.h"
#include "config.h"

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

	pthread_t plpThread, pcpThread;

	pthread_create(&plpThread, NULL, &IniciarPlp, NULL);
	pthread_create(&pcpThread, NULL, &IniciarPcp, NULL);

	pthread_join(plpThread, NULL);
	pthread_join(pcpThread, NULL);

	destruir_colas();
	destruir_config();

	return EXIT_SUCCESS;
}
