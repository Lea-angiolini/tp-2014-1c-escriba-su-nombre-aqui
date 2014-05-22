#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdlib.h>
#include <stdint.h>
#include "log.h"
#include "parser/parser.h"
#include "pcb.h"

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

/*******************************************************************/

//KernelPLP->UMV
typedef struct {
	socket_header header;
	uint32_t codeSegmentSize;//Segmento codigo literal ansisop
	uint32_t stackSegmentSize;
	uint32_t etiquetasSegmentSize; //Segmento de indice de etiquetas
	uint32_t instruccionesSegmentSize; //Segmento de indice de codigo
}  __attribute__((packed)) socket_pedirMemoria ;

//UMV->KernelPLP
typedef struct {
	socket_header header;
	uint32_t codeSegment;
	uint32_t stackSegment;
	uint32_t codeIndex;
	uint32_t etiquetaIndex;
}  __attribute__((packed)) socket_umvpcb ;



/*******************************************************************/

//KernelPCP->CPU
typedef struct {
	socket_header header; //header.code = 'c'
	uint32_t quantum;
	uint32_t retardo;
}  __attribute__((packed)) socket_cpucfg ;

//KernelPCP<->CPU
typedef struct {
	socket_header header; //header.code = 'p'
	pcb_t pcb;
}  __attribute__((packed)) socket_pcb;

//System Calls (sc)
//CPU<->KernelPCP
typedef struct {
	socket_header header; //header.code = 'o'
	char identificador[100];
	int32_t valor; //Se llena solo para la respuesta
}  __attribute__((packed)) socket_scObtenerValor;

//CPU->KernelPCP (necesita respuesta del kernel?)
typedef struct {
	socket_header header; //header.code = 'g'
	char identificador[100];
	int32_t valor;
}  __attribute__((packed)) socket_scGrabarValor;

//CPU->KernelPCP
typedef struct {
	socket_header header; //header.code = 'w'
	char identificador[100];
}  __attribute__((packed)) socket_scWait;

//CPU->KernelPCP
typedef struct {
	socket_header header; //header.code = 's'
	char identificador[100];
}  __attribute__((packed)) socket_scSignal;

//CPU->KernelPCP
typedef struct {
	socket_header header; //header.code = 'i'
	char identificador[100];
	uint32_t unidades;
}  __attribute__((packed)) socket_scIO;



//CPU->KernelPCP
typedef struct {
	socket_header header; //header.code = 'k'
	char texto[255];
	int programaSocket;
}  __attribute__((packed)) socket_imprimirTexto;



/*******************************************************************/

//CPU->UMV


typedef struct {
	socket_header header;
	uint32_t pdi;
	uint32_t base;
	uint32_t offset;
	uint32_t length;
	char data[100];
}  __attribute__((packed)) socket_guardarEnMemoria;


typedef struct {
	socket_header header;
	uint32_t pdi;
	uint32_t status;
}  __attribute__((packed)) socket_RespuestaGuardarEnMemoria;


typedef struct {
	socket_header header;
	uint32_t pdi;
	uint32_t base;
	uint32_t offset;
	uint32_t length;
}  __attribute__((packed)) socket_leerMemoria;


typedef struct {
	socket_header header;
	uint32_t pdi;
	uint32_t status;
	char data[100];
}  __attribute__((packed)) socket_RespuestaLeerMemoria;





typedef struct {
	uint32_t pdi;
	socket_header header;
	uint32_t numero_linea_Codigo;
}  __attribute__((packed)) socket_obtenerLineaCodigo;


typedef struct {
	socket_header header;
	uint32_t pdi;
	uint32_t numero_linea_Codigo;
	char lineaCodigo[1024];
}  __attribute__((packed)) socket_responderLineaCodigo;




/*******************************************************************/

int conectar					(char *ip, int port, t_log * log);
int crearYbindearSocket			(int puerto, t_log * log);
int crearServidor				(int puerto, void* (*fn_nuevo_cliente)( void * socket ), t_log * log);
int crearServidorNoBloqueante	(int puerto, bool (*fn_nuevo_mensaje)(void *socket), t_log * log);


void *	enviarYRecibirPaquete	( int socket, void * mensaje, uint32_t sizeSend, uint32_t sizeReceive, char sendCode, char receiveCode, t_log * logger );
int		enviarPaquete			( int socket, void * mensaje, uint32_t sizeSend, char sendCode, t_log * logger );
void *	recibirPaquete			( int socket, uint32_t sizeReceive, char receiveCode, t_log * logger );


#endif /* SOCKETS_H_ */
