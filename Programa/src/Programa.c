#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/sendfile.h>

#include "commons/config.h"
#include "commons/sockets.h"
#include "commons/tools.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Programa ScriptAnsiSOp\n");
		return EXIT_SUCCESS;
	}

	t_log *log = log_create("log.txt", "Programa", 1, LOG_LEVEL_TRACE);

	t_config *config = config_create(getenv("ANSISOP_CONFIG"));
	if( !config_has_property(config, "IP") || !config_has_property(config, "Puerto") ) {
		log_error(log, "Configuracion invalida");
		return EXIT_SUCCESS;
	}

	FILE *script = fopen(argv[1], "r"); //argv[1] = nombre del script a correr
	off_t scriptBase = 0;
	size_t scriptSize;

	if (script == NULL ) {
		log_error(log, "Error al leer el script: %s", argv[1]);
		return EXIT_SUCCESS;
	}

	//Obteniendo tamaño del archivo
	fseek(script, 0, SEEK_END);
	scriptSize = ftell(script);
	fseek(script, 0, SEEK_SET);

	int sock = conectar(config_get_string_value(config, "IP"), config_get_int_value(config, "Puerto"), log);

	if (sock == -1) {
		log_error(log, "No se pudo conectar al kernel");
		return EXIT_SUCCESS;
	}

	log_info(log, "Conectado al kernel");

	socket_header header;
	header.size = sizeof(header)+scriptSize;

	if( send(sock, &header, sizeof(header), 0) < 0 || sendfile(sock, script->_fileno, &scriptBase, scriptSize) < 0) {
		log_error(log, "No se pudo enviar el archivo");
		return EXIT_SUCCESS;
	}

	socket_msg msg;

	while (recv(sock, &msg, sizeof(msg), MSG_WAITALL) > 0)
		log_info(log, "%s", msg.msg);

	log_info(log, "Desconectado del servidor");

	close(sock);
	fclose(script);
	config_destroy(config);
	log_destroy(log);

	return EXIT_SUCCESS;
}


























