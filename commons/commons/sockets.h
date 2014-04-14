#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdlib.h>
#include "log.h"

typedef struct {
	unsigned char code;
	size_t size;
} socket_header;

typedef struct {
	socket_header header;
	char msg[255];
} socket_msg;


int conectar(char *ip, int port, t_log * log);
int crearServidor(int puerto, void* (*fn_nuevo_cliente)( void * socket ), t_log * log);

#endif /* SOCKETS_H_ */
