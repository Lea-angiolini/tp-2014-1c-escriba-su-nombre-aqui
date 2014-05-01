#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "commons/sockets.h"
#include "commons/config.h"
#include "commons/collections/list.h"
#include "Segmento.h"

typedef struct {
	uint32_t pid;
	Segmento * stack;
	Segmento * script;
	Segmento * etiquetas;
	Segmento * instrucciones;

} Programa;


Programa * crearPrograma(uint32_t pid, void * script, void * etiquetas,
		void * instrucciones_serializado,
		socket_pedirMemoria * segmentosAreservar);

Segmento * crearDireccionesVirtuales(Segmento * segmento,
		uint32_t tamanioSegmento, uint32_t finVirtualDelAnterior);

socket_umvpcb crearEstructuraParaPCB( Programa * programa);
