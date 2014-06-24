#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "config.h"
#include "handshake.h"
#include "mocks.h"

t_log * logger;
FILE *mensajesUMV;

pthread_t threadConsola;
pthread_t threadConexiones;
pthread_rwlock_t lockEscrituraLectura;


int main(int argc, char * argv[]) {

	if (argc != 2) {
		printf("Modo de empleo: ./UMV config.cfg\n");
		return EXIT_SUCCESS;
	}
	mensajesUMV = fopen("mensajes_UMV.txt", "a");
	logger = log_create("log.txt", "UMV", 1, LOG_LEVEL_TRACE);

	fprintf(mensajesUMV, "Iniciando UMV...\n");
	//log_info(logger, "Iniciando UMV...");

	if( !cargar_config(argv[1]) ) {
		printf("Archivo de configuracion invalido\n");
		return EXIT_SUCCESS;
	}

	pthread_create(&threadConsola, NULL, iniciarConsola, NULL);
	pthread_create(&threadConexiones, NULL, crearConexiones, NULL);
	pthread_rwlock_init(&lockEscrituraLectura, NULL);

	pthread_join(threadConexiones, NULL);
	fprintf(mensajesUMV, "Finalizando la consola...\n");
	//log_info(logger, "Finalizando la consola ...");
	pthread_cancel(threadConsola);

	destruir_config();
	pthread_rwlock_destroy(&lockEscrituraLectura);

	fprintf(mensajesUMV, "Finalizando UMV...\n");
	//log_info( logger, "Finalizando UMV...");
	fclose(mensajesUMV);
	//log_destroy(logger);

	return EXIT_SUCCESS;
}
