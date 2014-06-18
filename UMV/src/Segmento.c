#include "Segmento.h"
#include "config.h"


Segmento * new_Segmento( uint32_t inicio, uint32_t fin ) {

	Segmento * segmento		= malloc( sizeof( Segmento ) );
	segmento->inicioReal	= inicio;
	segmento->finReal		= fin - 1;

	segmento->finVirtual	= 0;
	segmento->inicioVirtual = 0;

	return segmento;

}


//TODO chequear segmentation fault
void * memCopi( Segmento * segmento, uint32_t offset, void * valor, uint32_t length ) {
	return memcpy( memoria + segmento->inicioReal + offset, valor, length );
}

//TODO chequear segmentation fault
void * memLeer( Segmento * segmento, void * destino, uint32_t offset, uint32_t length ) {
	return memcpy( destino, memoria + segmento->inicioReal + offset, length );
}
