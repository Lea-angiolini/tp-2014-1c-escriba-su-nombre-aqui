#include <stdint.h>

#ifndef UMV_H_
#define UMV_H_


char * solicitarLineaPrograma( uint32_t programCounter );

int enviarCambioContexto( uint32_t pid );
int enviarFinPrograma( uint32_t pid );
int enviarFinQuantum( uint32_t pid );


int armarDiccionarioVariablesDelStack() ;
int obtenerStack();


#endif /* UMV_H_ */








