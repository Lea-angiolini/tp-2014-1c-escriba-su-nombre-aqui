#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <sys/socket.h>
#include <sys/sendfile.h>

#include "commons/config.h"
#include "commons/sockets.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Programa ScriptAnsiSOp\n");
		return EXIT_SUCCESS;
	}

	t_config *config = config_create("$ANSISOP_CONFIG");

	FILE *script = fopen(argv[1], "r"); //argv[1] = nombre del script a correr
	size_t scriptSize;

	if (script == NULL ) {
		printf("Error al leer el script: %s\n", argv[1]);
		return EXIT_SUCCESS;
	}

	//Obteniendo tamaño del archivo
	fseek(script, 0, SEEK_END);
	scriptSize = ftell(script);
	fseek(script, 0, SEEK_SET);

	int sock = conectar(config_get_string_value(config, "IP"), config_get_int_value(config, "Puerto"));

	if (sock == -1) {
		printf("No se pudo conectar al servidor.\n");
		return EXIT_SUCCESS;
	}

	//send(sock, header, sizeof(header);

	if( sendfile(sock, fileno(script), 0, scriptSize) < 0 ) {
		printf("No se pudo enviar el archivo.\n");
		return EXIT_SUCCESS;
	}

	//while( recv(sock, output) ) printf(“%s\n”, output);

	close(sock);
	fclose(script);
	config_destroy(config);

	return EXIT_SUCCESS;
}
