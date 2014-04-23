#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdlib.h>
#include <stdint.h>
#include "log.h"
#include "parser/parser.h"

/////////////////////////////////////////////////////////////////
// PAQUETES DE DATOS PARA COMUNICACION ENTRE PROGRAMAS
/////////////////////////////////////////////////////////////////
typedef struct {
	unsigned char code;
	size_t size;
} __attribute__((packed)) socket_header  ;

typedef struct {
	socket_header header;
	char msg[255];
}  __attribute__((packed)) socket_msg ;

typedef struct {
	socket_header header;
	bool valor; //false = no se pudo, true = se puede
}  __attribute__((packed)) socket_respuesta ;

//KernelPLP->UMV
typedef struct {
	socket_header header;
	uint32_t segmentSize[4];
}  __attribute__((packed)) socket_pedirMemoria ;





/*******************************************************************/

//CPU->UMV
typedef struct {
	socket_header header;
	t_nombre_variable identificador_variable;
	t_valor_variable valor;
}  __attribute__((packed)) socket_guardarVariable ;


typedef struct {
	socket_header header;
	t_nombre_variable identificador_variable;
}  __attribute__((packed)) socket_definirVariable ;


typedef struct {
	socket_header header;
	t_nombre_variable obtenerPosicionVariable;
	t_puntero direccion_variable;
}  __attribute__((packed)) socket_obtenerPosicionVariable ;

typedef struct {
	socket_header header;
	uint32_t numero_linea_Codigo;
}  __attribute__((packed)) socket_obtenerLineaCodigo;

typedef struct {
	socket_header header;
	uint32_t numero_linea_Codigo;
	char lineaCodigo[1024];
}  __attribute__((packed)) socket_responderLineaCodigo;




/////////////////////////////////////////////////////////////////

int conectar(char *ip, int port, t_log * log);
int crearYbindearSocket(int puerto, t_log * log);
int crearServidor(int puerto, void* (*fn_nuevo_cliente)( void * socket ), t_log * log);
int crearServidorNoBloqueante(int puerto, bool (*fn_nuevo_mensaje)(void *socket), t_log * log);
#endif /* SOCKETS_H_ */
