#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdlib.h>
#include <stdint.h>
#include "log.h"

/////////////////////////////////////////////////////////////////
// PAQUETES DE DATOS PARA COMUNICACION ENTRE PROGRAMAS
/////////////////////////////////////////////////////////////////
typedef struct {
	unsigned char code;
	size_t size;
} socket_header  __attribute__((packed));

typedef struct {
	socket_header header;
	char msg[255];
} socket_msg  __attribute__((packed));

typedef struct {
	socket_header header;
	bool valor; //false = no se pudo, true = se puede
} socket_respuesta  __attribute__((packed));

//KernelPLP->UMV
typedef struct {
	socket_header header;
	uint32_t segmentSize[4];
} socket_pedirMemoria  __attribute__((packed));

/////////////////////////////////////////////////////////////////

int conectar(char *ip, int port, t_log * log);
int crearYbindearSocket(int puerto, t_log * log);
int crearServidor(int puerto, void* (*fn_nuevo_cliente)( void * socket ), t_log * log);
int crearServidorNoBloqueante(int puerto, bool (*fn_nuevo_mensaje)(void *socket), t_log * log);
#endif /* SOCKETS_H_ */
