#include <stdbool.h>

#include "Segmento.h"

#include "commons/collections/list.h"


#ifndef MEMORIA_H_
#define MEMORIA_H_



bool segmentoEsAnterior( void * seg1, void * seg2 );
t_list * crearListaEspacioDisponible();

Segmento * crearYllenarSegmento( uint32_t tamanio, void * segmento );
Segmento * crearSegmento		( uint32_t tamanio );
Segmento * crearSegmentoFistFit	( t_list * huequitos, uint32_t tamanio );
Segmento * crearSegmentoWorstFit( t_list * huequitos, uint32_t tamanio );
Segmento * buscarSegmentoEnTabla( uint32_t idSeg);
void borrarSegmento( Segmento * segmentoABorrar );
void moverSegmento(Segmento * segmento, uint32_t posicion) ;
int tamanioSegmento(Segmento * segmento);

void ordenarTablaSegmentos();
void compactar();
uint32_t memoriaOcupada();
uint32_t memoriaLibre();

uint32_t solicitarPosicionDeMemoria( uint32_t programa, uint32_t base, uint32_t offset, uint32_t tamanio);
void mostrarCaracteres( uint32_t cantidad, unsigned char * mem);
uint32_t escribirPosicionDeMemoria( uint32_t programa, uint32_t base, uint32_t offset, uint32_t tamanio, uint32_t  buffer[]);
bool chequearSegmentatiosFault( Segmento * segmento, uint32_t offset, uint32_t tamanio);



#endif /* MEMORIA_H_ */
