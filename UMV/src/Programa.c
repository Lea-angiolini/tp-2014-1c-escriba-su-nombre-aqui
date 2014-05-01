#include "Programa.h"
#include "memoria.h"

extern t_list * programas;
extern t_config * umvConfig;

socket_umvpcb  new_Program(uint32_t pid, void * script, void * etiquetas,
		void * instrucciones_serializado,
		socket_pedirMemoria * tamanioSegmentos) {

	Programa * programa = malloc( sizeof(Programa));
	programa->pid = pid;

	uint32_t tamanioStack, tamanioScript, tamanioEtiquetas,tamanioInstrucciones;
	tamanioStack = config_get_int_value(umvConfig, "TAMANIOSTACK");
	tamanioScript = tamanioSegmentos->codeSegmentSize;
	tamanioEtiquetas = tamanioSegmentos->etiquetasSegmentSize;
	tamanioInstrucciones = tamanioSegmentos->instruccionesSegmentSize;

	programa->stack = crearSegmento(tamanioStack);
	programa->stack->inicioVirtual = 0;
	programa->stack->finVirtual = tamanioStack - 1;

	programa->script = crearYllenarSegmento(tamanioScript, script);
	crearDireccionesVirtuales(programa->script, tamanioScript,
			programa->stack->finVirtual);

	programa->etiquetas = crearYllenarSegmento(tamanioEtiquetas, etiquetas);
	crearDireccionesVirtuales(programa->etiquetas, tamanioEtiquetas,
			programa->script->finVirtual);

	programa->instrucciones = crearYllenarSegmento(tamanioInstrucciones,
			instrucciones_serializado);
	crearDireccionesVirtuales(programa->instrucciones, tamanioInstrucciones,
			programa->etiquetas->finVirtual);

	list_add(programas, programa);

	socket_umvpcb datosSegmentos;
	datosSegmentos.stackSegment = programa->stack->inicioVirtual;
	datosSegmentos.codeSegment = programa->script->inicioVirtual;
	datosSegmentos.etiquetaIndex = programa->etiquetas->inicioVirtual;
	datosSegmentos.codeIndex = programa->instrucciones->inicioVirtual;

	free(programa);
	return datosSegmentos;

}

Segmento * crearDireccionesVirtuales(Segmento * segmento, uint32_t tamanioSegmento, uint32_t finVirtualDelAnterior) {

	segmento->inicioVirtual = finVirtualDelAnterior + 1;
	segmento->finVirtual = segmento->inicioVirtual + (tamanioSegmento - 1);

	return segmento;
}

