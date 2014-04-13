#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <sys/socket.h>
//#include <sys/sendfile.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "commons/config.h"

int conectar(char *ip, int port) {
	//PF_INET: IPv4
	//SOCK_STREAM: Los datos no se pierden ni se duplican
	//IPPROTO_TCP: TCP
	int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock < 0)
		return -1; //No se pudo crear el descriptor a un archivo

	struct sockaddr_in sockaddress;

	memset(&sockaddress, 0, sizeof(struct sockaddr_in));
	sockaddress.sin_family = AF_INET;
	sockaddress.sin_addr.s_addr = inet_addr(ip);
	sockaddress.sin_port = htons(port);

	if( connect(sock, (struct sockaddr*)&sockaddress, sizeof(struct sockaddr)))
		return -1;  //No se pudo conectar

	return sock;
}

char* LeerArchivo(char *archivo, size_t *size) {
	FILE *f = fopen(archivo, "r");

	if (f == NULL )
		return NULL ; //No pudo abrir el archivo

	//Obteniendo tamaño del archivo
	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *contenido = malloc(*size * sizeof(char));
	if (contenido == NULL )
		return NULL ; //Error de memoria

	if (fread(contenido, 1, *size, f) != *size)
		return NULL ; //No pudo leer todo el archivo

	fclose(f);

	return contenido;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Programa ScriptAnsiSOp\n");
		return EXIT_SUCCESS;
	}

	size_t scriptSize;
	char *script = LeerArchivo(argv[1], &scriptSize); //argv[1] = nombre del script a correr

	if (script == NULL) {
		printf("Error al leer el script: %s\n", argv[1]);
		return EXIT_SUCCESS;
	}

	t_config *config = config_create("$ANSISOP_CONFIG");

	int sock = conectar(config_get_string_value(config, "IP"), config_get_int_value(config, "Puerto"));

	if (sock == -1) {
		printf("No se pudo conectar al servidor.\n");
		return EXIT_SUCCESS;
	}

	send(sock, &script, scriptSize, 0);

	//while( recv(sock, output) ) printf(“%s\n”, output);

	close(sock);
	config_destroy(config);
	free(script);

	return EXIT_SUCCESS;
}
