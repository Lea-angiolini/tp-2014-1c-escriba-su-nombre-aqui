#include <stdio.h>
#include <stdlib.h>

#include "plp.h"
#include "pcp.h"
#include "io.h"
#include "colas.h"

#include "commons/config.h"
//#include "commons/collections/list.h"

#include <stdint.h>
#include <stdbool.h>

t_config *config;
//t_list *dispositivos_io;

const char cofig_properties[][25] = {
	"PUERTO_PROG", "PUERTO_CPU", "QUANTUM", "RETARDO",
	"MULTIPROGRAMACION", "SEMAFOROS", "VALOR_SEMAFORO",
	"HIO", "ID_HIO", "VARIABLES_COMPARTIDAS",
	"IP_UMV", "PUERTO_UMV"
};

bool validar_configuracion()
{
	bool ret = true;
	int elements = sizeof(cofig_properties)/sizeof(cofig_properties[0]);
	int i;

	for(i = 0; i < elements; i++) {
		if( !config_has_property(config, &cofig_properties[i]) ) {
			ret = false;
			break;
		}
	}

	return ret;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Kernel config.cfg\n");
		return EXIT_SUCCESS;
	}

	config = config_create(argv[1]);
	if( !validar_configuracion() ) {
		printf("Archivo de configuracion invalido\n");
		return EXIT_SUCCESS;
	}

	//char** hioId = config_get_array_value(config, "ID_HIO");
	//char** hioRetardo = config_get_array_value(config, "HIO");

	//dispositivos_io = armar_lista_dispositivos(hioId,hioRetardo);

	crearColas();

	pthread_t plpThread, pcpThread;

	pthread_create(&plpThread, NULL, &IniciarPlp, NULL);
	pthread_create(&pcpThread, NULL, &IniciarPcp, NULL);

	pthread_join(plpThread, NULL);
	pthread_join(pcpThread, NULL);


	destruirColas();
	config_destroy(config);

	return EXIT_SUCCESS;
}
