#ifndef UMV_H_
#define UMV_H_

#include <stdint.h>
#include <stdbool.h>

bool crearConexionUMV();

char * solicitarLineaPrograma( uint32_t programCounter );
uint32_t obtenerLineaDeLabel( t_nombre_etiqueta t_nombre_etiqueta );

int enviarCambioContexto( uint32_t pid );
int enviarFinPrograma( uint32_t pid );
int enviarFinQuantum( uint32_t pid );




extern int socketUMV;

#endif /* UMV_H_ */








