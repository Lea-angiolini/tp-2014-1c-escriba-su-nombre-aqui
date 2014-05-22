#ifndef UMV_H_
#define UMV_H_

#include <stdint.h>
#include <stdbool.h>

bool crearConexionUMV();

char * solicitarLineaPrograma( uint32_t programCounter );

int enviarCambioContexto( uint32_t pid );
int enviarFinPrograma( uint32_t pid );
int enviarFinQuantum( uint32_t pid );


int obtenerContextStack();
int guardarStack();

extern int socketUMV;

#endif /* UMV_H_ */








