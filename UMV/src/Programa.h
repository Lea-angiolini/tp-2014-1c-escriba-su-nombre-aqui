#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "commons/sockets.h"
#include "commons/config.h"
#include "commons/collections/list.h"
#include "Segmento.h"

#ifndef PROGRAMA_H_
#define PROGRAMA_H_

typedef struct {
	uint32_t pid;
	Segmento * stack;
	Segmento * script;
	Segmento * etiquetas;
	Segmento * instrucciones;

} Programa;


Programa *  crearPrograma(uint32_t pid, void * script, void * etiquetas, void * instrucciones_serializado, uint32_t tamanioScript, uint32_t tamanioEtiquetas, uint32_t tamanioInstrucciones, uint32_t tamanioStack );

Segmento * crearDireccionesVirtuales(Segmento * segmento,
		uint32_t tamanioSegmento, uint32_t finVirtualDelAnterior);

socket_umvpcb crearEstructuraParaPCB( Programa * programa);

Programa * buscarPrograma( uint32_t pdi);

Segmento * buscarSegmentoEnPrograma( Programa * programa, uint32_t offset);

bool destruirPrograma( uint32_t pid);

#endif /*PROGRAMA_H_*/
