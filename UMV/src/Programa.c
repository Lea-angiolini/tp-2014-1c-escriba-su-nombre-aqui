#include "Programa.h"
#include "memoria.h"


extern t_list * programas;




Programa *  crearPrograma(uint32_t pid, void * script, void * etiquetas,
		void * instrucciones_serializado,
		socket_pedirMemoria * segmentosAreservar){


	Programa * programa = malloc( sizeof( Programa));


	programa->pid = pid;

	uint32_t tamanioStack, tamanioScript, tamanioEtiquetas,tamanioInstrucciones;
	tamanioScript = segmentosAreservar->codeSegmentSize;
	tamanioStack = segmentosAreservar->stackSegmentSize;
	tamanioEtiquetas = segmentosAreservar->etiquetasSegmentSize;
	tamanioInstrucciones = segmentosAreservar->instruccionesSegmentSize;

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

	return programa;

}

socket_umvpcb crearEstructuraParaPCB( Programa * programa){

	    socket_umvpcb datosSegmentos;

		datosSegmentos.stackSegment = programa->stack->inicioVirtual;
		datosSegmentos.codeSegment = programa->script->inicioVirtual;
		datosSegmentos.etiquetaIndex = programa->etiquetas->inicioVirtual;
		datosSegmentos.codeIndex = programa->instrucciones->inicioVirtual;

		return datosSegmentos;
}

Segmento * crearDireccionesVirtuales(Segmento * segmento, uint32_t tamanioSegmento, uint32_t finVirtualDelAnterior) {

	segmento->inicioVirtual = finVirtualDelAnterior + 1;
	segmento->finVirtual = segmento->inicioVirtual + (tamanioSegmento - 1);

	return segmento;
}

Programa * buscarPrograma( uint32_t pdi){

	uint32_t tamanioProgramas;
	tamanioProgramas = list_size( programas);
	int i = 0;
	for(i = 0; i <= tamanioProgramas; i++){
		Programa * programa = list_get(programas, i);
		if(programa->pid == pdi)
			return programa;
	}
	return NULL;
}

Segmento * buscarSegmentoEnPrograma( Programa * programa, uint32_t offset){

	if( (offset >= programa->stack->inicioVirtual) && (offset <= programa->stack->finVirtual))
		return programa->stack;

	if( (offset >= programa->script->inicioVirtual) && (offset <= programa->script->finVirtual))
			return programa->script;

	if( (offset >= programa->etiquetas->inicioVirtual) && (offset <= programa->etiquetas->finVirtual))
			return programa->etiquetas;

	if( (offset >= programa->instrucciones->inicioVirtual) && (offset <= programa->instrucciones->finVirtual))
			return programa->instrucciones;
	else{
		return NULL;
	}
}
