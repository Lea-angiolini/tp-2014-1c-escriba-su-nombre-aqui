#include <stdint.h>
#include <stdlib.h>

#include "Segmento.h"

#include "commons/log.h"

extern t_log * logger;


Segmento * new_Segmento( uint32_t inicio, uint32_t fin ) {

	Segmento * segmento = malloc( sizeof( Segmento ) );
	segmento->inicioReal = inicio;
	segmento->finReal = fin;

	segmento->finVirtual = 0;
	segmento->inicioVirtual = 0;

	return segmento;

}
