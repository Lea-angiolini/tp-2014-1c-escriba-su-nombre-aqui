#ifndef UMV_H_
#define UMV_H_

#include <stdint.h>

char * solicitarLineaPrograma( uint32_t programCounter );

int enviarCambioContexto( uint32_t pid );
int enviarFinPrograma( uint32_t pid );
int enviarFinQuantum( uint32_t pid );


int obtenerContextStack();
int guardarStack();

#endif /* UMV_H_ */








