#include "Programa.h"
#include "memoria.h"
#include "config.h"

extern t_log * logger;
extern pthread_rwlock_t lockEscrituraLectura;

Programa * crearPrograma(uint32_t pid, void * script, void * etiquetas,
		void * instrucciones_serializado, uint32_t tamanioScript,
		uint32_t tamanioEtiquetas, uint32_t tamanioInstrucciones,
		uint32_t tamanioStack) {
	pthread_rwlock_rdlock(&lockEscrituraLectura);
	Programa * programa = malloc(sizeof(Programa));

	programa->pid = pid;

	srand( time(NULL));

	programa->stack = crearSegmento(tamanioStack);
		crearDireccionesVirtuales( programa->stack, tamanioStack, 0);

	/*	programa->stack->inicioVirtual = 0;
		programa->stack->finVirtual = tamanioStack - 1;*/

		programa->script = crearYllenarSegmento(tamanioScript, script);
		crearDireccionesVirtuales(programa->script, tamanioScript,
				programa->stack->finVirtual);

		programa->etiquetas = crearYllenarSegmento(tamanioEtiquetas, etiquetas);
		if( programa->script->finVirtual == SEGMENTOVACIO ){
			crearDireccionesVirtuales( programa->etiquetas, tamanioEtiquetas, programa->stack->finVirtual);
		}else{
			crearDireccionesVirtuales( programa->etiquetas, tamanioEtiquetas, programa->script->finVirtual);
		}
		/*if( programa->etiquetas->inicioReal ==SEGMENTOVACIO){
			crearDireccionesVirtuales( programa->etiquetas, tamanioEtiquetas, 0);
			}else{
				crearDireccionesVirtuales(programa->etiquetas, tamanioEtiquetas,
				programa->script->finVirtual);
				}*/

		programa->instrucciones = crearYllenarSegmento(tamanioInstrucciones,
				instrucciones_serializado);
		if( programa->etiquetas->finVirtual == SEGMENTOVACIO ){
			crearDireccionesVirtuales( programa->instrucciones, tamanioInstrucciones, programa->script->finVirtual);
		}else{
			crearDireccionesVirtuales( programa->instrucciones, tamanioInstrucciones, programa->etiquetas->finVirtual);
		}


		/*if(programa->etiquetas->inicioVirtual == SEGMENTOVACIO){
			crearDireccionesVirtuales( programa->instrucciones, tamanioInstrucciones, programa->script->finVirtual);
		}else{
			crearDireccionesVirtuales(programa->instrucciones, tamanioInstrucciones,
				programa->etiquetas->finVirtual);
			}*/
		list_add(programas, programa);
		printf( "%d\t\t%d\n", programa->stack->inicioVirtual,programa->stack->finVirtual);
		printf( "%d\t\t%d\n", programa->script->inicioVirtual,programa->script->finVirtual);
		printf( "%d\t\t%d\n", programa->etiquetas->inicioVirtual,programa->etiquetas->finVirtual);
		printf( "%d\t\t%d\n", programa->instrucciones->inicioVirtual,programa->instrucciones->finVirtual);

		pthread_rwlock_unlock(&lockEscrituraLectura);

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
	printf("Mi tamaño es %d!\n", tamanioSegmento);
	if( tamanioSegmento == 0){
		segmento->inicioVirtual = SEGMENTOVACIO;
		segmento->finVirtual = SEGMENTOVACIO;
	}else{
		uint32_t seed = finVirtualDelAnterior + 1;
		segmento->inicioVirtual = rand() % 417 + seed;
		printf("mi inicioVirtual%d \n", segmento->inicioVirtual);
		segmento->finVirtual = segmento->inicioVirtual + tamanioSegmento - 1;
		}

	return segmento;


	/*if( segmento->inicioReal == SEGMENTOVACIO){
		segmento->inicioVirtual = SEGMENTOVACIO;
	}else{

	segmento->inicioVirtual = finVirtualDelAnterior + 1;
	segmento->finVirtual = segmento->inicioVirtual + (tamanioSegmento - 1);
	}
	return segmento;*/
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
	log_info(logger, "La base que estoy buscando es %d 2", base);

	if (base == programa->stack->inicioVirtual){
		log_info(logger,"Es el stack!");
		return programa->stack;}
	if (base == programa->script->inicioVirtual){
		log_info(logger,"Es el script!");

		return programa->script;}
	if (base == programa->etiquetas->inicioVirtual){
		log_info(logger,"Es el de etiquetas!");

		return programa->etiquetas;}
	if (base == programa->instrucciones->inicioVirtual){
		log_info(logger,"Es el de instrucciones!");

		return programa->instrucciones;}
	return NULL ;

}


bool destruirPrograma( Programa * programa ){
	bool matchearPrograma(Programa *nodoPrograma) {
			return nodoPrograma->pid == programa->pid;
		}

		list_remove_by_condition( programas, matchearPrograma);

		if( programa != NULL){
			log_info( logger, "Destruyendo programa con pid: %d", programa->pid);
			pthread_rwlock_rdlock(&lockEscrituraLectura);
			borrarSegmento( programa->stack );
			borrarSegmento( programa->script );
			borrarSegmento( programa->etiquetas );
			borrarSegmento( programa->instrucciones );
			pthread_rwlock_unlock(&lockEscrituraLectura);
			free( programa );

			return true;
		}else{
			log_error( logger, "El programa con pid: %d no se ha podido destruir", programa->pid);
			return false;
		}
}

