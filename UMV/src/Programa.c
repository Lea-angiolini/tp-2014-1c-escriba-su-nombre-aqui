#include "Programa.h"
#include "memoria.h"

extern t_list * programas;
extern t_log * logger;

Programa * crearPrograma(uint32_t pid, void * script, void * etiquetas,
		void * instrucciones_serializado, uint32_t tamanioScript,
		uint32_t tamanioEtiquetas, uint32_t tamanioInstrucciones,
		uint32_t tamanioStack) {

	Programa * programa = malloc(sizeof(Programa));

	programa->pid = pid;

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

socket_umvpcb crearEstructuraParaPCB(Programa * programa) {

	socket_umvpcb datosSegmentos;

	datosSegmentos.stackSegment = programa->stack->inicioVirtual;
	datosSegmentos.codeSegment = programa->script->inicioVirtual;
	datosSegmentos.etiquetaIndex = programa->etiquetas->inicioVirtual;
	datosSegmentos.codeIndex = programa->instrucciones->inicioVirtual;

	return datosSegmentos;
}

Segmento * crearDireccionesVirtuales(Segmento * segmento,
		uint32_t tamanioSegmento, uint32_t finVirtualDelAnterior) {

	if( segmento->inicioReal == SEGMENTOVACIO){
		segmento->inicioVirtual = SEGMENTOVACIO;
	}

	segmento->inicioVirtual = finVirtualDelAnterior + 1;
	segmento->finVirtual = segmento->inicioVirtual + (tamanioSegmento - 1);

	return segmento;
}

Programa * buscarPrograma(uint32_t pid) {
	bool matchearPrograma(Programa *nodoPrograma) {
		return nodoPrograma->pid == pid;
	}

	return list_find(programas, matchearPrograma);

	//Seria mas facil asi, que ya se cuenta con una funcion copada en la commons

	//uint32_t tamanioProgramas;
	//tamanioProgramas = list_size( programas);
	//int i = 0;
	//for(i = 0; i <= tamanioProgramas; i++){
	//	Programa * programa = list_get(programas, i);
	//	if(programa->pid == pdi)
	//		return programa;
	//}
	//return NULL;
}

Segmento * buscarSegmentoEnPrograma(Programa * programa, uint32_t base) {

	if (base == programa->stack->inicioVirtual)
		return programa->stack;
	if (base == programa->script->inicioVirtual)
		return programa->script;
	if (base == programa->etiquetas->inicioVirtual)
		return programa->etiquetas;
	if (base == programa->instrucciones->inicioVirtual)
		return programa->instrucciones;
	return NULL ;

}

void destruirTodosLosProgramas(){
	uint32_t i;
	for( i = 0; i < (list_size( programas) - 1); i++){
		Programa * programa = list_get( programas, i);
		destruirPrograma( programa);
	}

}
bool destruirPrograma( Programa * programa ){
	bool matchearPrograma(Programa *nodoPrograma) {
			return nodoPrograma->pid == programa->pid;
		}

		list_remove_by_condition( programas, matchearPrograma);

		if( programa != NULL){
			log_info( logger, "Destruyendo programa con pid: %d", programa->pid);

			borrarSegmento( programa->stack );
			borrarSegmento( programa->script );
			borrarSegmento( programa->etiquetas );
			borrarSegmento( programa->instrucciones );
			free( programa );

			return true;
		}else{
			log_error( logger, "El programa con pid: %d no se ha podido destruir", programa->pid);
			return false;
		}
}

