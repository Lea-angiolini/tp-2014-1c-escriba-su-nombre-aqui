#ifndef UMV_H_
#define UMV_H_

#include <stdint.h>
#include <stdbool.h>

#include "commons/parser/parser.h"

bool crearConexionUMV();

char * solicitarLineaPrograma();
uint32_t obtenerLineaDeLabel( t_nombre_etiqueta t_nombre_etiqueta );
void eliminarSaltoLinea(char * linea);

bool escribirStack(uint32_t offset, uint32_t length, void * data);
bool escribirMemoria(uint32_t base, uint32_t offset, uint32_t length, void * data);

void * leerMemoria(uint32_t base, uint32_t offset, uint32_t length);
void * leerStack(uint32_t offset, uint32_t length);


bool enviarCambioContexto( uint32_t pid );
bool enviarFinPrograma( uint32_t pid );
bool enviarFinQuantum( uint32_t pid );
bool obtenerEtiquetas();



extern int socketUMV;

#endif /* UMV_H_ */








