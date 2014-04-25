#include "commons/collections/list.h"


#ifndef MEMORIA_H_
#define MEMORIA_H_


t_list * crearListaEspacioDisponible();
uint32_t crearSegmento( uint32_t tamanio );

uint32_t crearSegmentoFistFit( t_list * huequitos, uint32_t tamanio );


#endif /* MEMORIA_H_ */
