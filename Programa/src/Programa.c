#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/sendfile.h>

#include "commons/config.h"
#include "commons/sockets.h"
#include "commons/tools.h"

int main(int argc, char *argv[]) {

	t_log * log =	log_create( "log.txt", "Programa", 1, LOG_LEVEL_TRACE );

	if (argc != 2) {
		printf("Modo de empleo: ./Programa ScriptAnsiSOp\n");
		return EXIT_SUCCESS;
	}

	t_config *config = config_create(getenv("ANSISOP_CONFIG"));

	FILE *script = fopen(argv[1], "r"); //argv[1] = nombre del script a correr
	off_t scriptBase = 0;
	size_t scriptSize;

	if (script == NULL ) {
		printf("Error al leer el script: %s\n", argv[1]);
		return EXIT_SUCCESS;
	}

	//Obteniendo tamaño del archivo
	fseek(script, 0, SEEK_END);
	scriptSize = ftell(script);
	fseek(script, 0, SEEK_SET);

	int sock = conectar(config_get_string_value(config, "IP"), config_get_int_value(config, "Puerto"), log);

	if (sock == -1) {
		printf("No se pudo conectar al servidor.\n");
		return EXIT_SUCCESS;
	}

	printf("Conectado al servidor.\n");

	socket_header header;
	header.size = scriptSize;

	if( send(sock, &header, sizeof(header), 0) < 0 || sendfile(sock, script->_fileno, &scriptBase, scriptSize) < 0) {
		printf("No se pudo enviar el archivo.\n");
		return EXIT_SUCCESS;
	}

	socket_msg msg;

	while (recv(sock, &msg, sizeof(msg), 0) > 0)
		printf("%s\n", msg.msg);

	printf("Desconectado del servidor.\n");

	close(sock);
	fclose(script);
	config_destroy(config);

	return EXIT_SUCCESS;
}


























