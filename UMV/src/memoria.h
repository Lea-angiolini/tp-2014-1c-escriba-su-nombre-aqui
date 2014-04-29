#include <stdbool.h>

#include "Segmento.h"

#include "commons/collections/list.h"


#ifndef MEMORIA_H_
#define MEMORIA_H_



bool segmentoEsAnterior( void * seg1, void * seg2 );
t_list * crearListaEspacioDisponible();

Segmento * crearSegmento		( uint32_t tamanio );
Segmento * crearSegmentoFistFit	( t_list * huequitos, uint32_t tamanio );
Segmento * crearSegmentoWorstFit( t_list * huequitos, uint32_t tamanio );

void borrarSegmento( Segmento * segmentoABorrar );
void moverSegmento(Segmento * segmento, int tamanio, int posicion);
int tamanioSegmento(Segmento * segmento);

void compactar();
uint32_t memoriaOcupada();
uint32_t memoriaLibre();





#endif /* MEMORIA_H_ */
