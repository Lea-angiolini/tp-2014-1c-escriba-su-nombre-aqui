#include <stdint.h>

#ifndef SEGMENTO_H_
#define SEGMENTO_H_


typedef struct {

	uint32_t id;

	uint32_t inicioVirtual;	//Estos son los que cree el programa
	uint32_t finVirtual;

	uint32_t inicioReal;	//Estos son los offset reales respecto al memoria
	uint32_t finReal;

} Segmento;


Segmento * new_Segmento( uint32_t inicio, uint32_t fin ) ;


#endif /* SEGMENTO_H_ */

