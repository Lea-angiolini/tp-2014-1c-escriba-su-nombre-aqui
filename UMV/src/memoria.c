#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Segmento.h"
#include "memoria.h"
#include "Consola.h"

#include "commons/collections/list.h"
#include "commons/log.h"
#include "commons/sockets.h"


extern t_list * tabla_segmentos;
extern uint32_t memoria_size;
extern void * memoria;
extern t_log * logger;

Segmento * crearYllenarSegmento( uint32_t tamanio, void * segmento ){ //TODO Habria que agregarle un id de tipo al segmento
		Segmento * segmentoAllenar = crearSegmento( tamanio);
		memCopi(segmentoAllenar, 0, segmento, tamanio);
		return segmentoAllenar;
}



Segmento * crearSegmento( uint32_t tamanio ) {

	log_info( logger, "Creando segmento, ahora hay %d", list_size( tabla_segmentos ) );

	Segmento * elNuevo = NULL;

	if( list_size( tabla_segmentos ) == 0 && memoria_size > tamanio ){

		elNuevo = new_Segmento( 0, tamanio );

	}else{

		t_list * huequitos = crearListaEspacioDisponible();
		elNuevo = crearSegmentoWorstFit( huequitos,  tamanio ) ;
		list_destroy( huequitos );

	}

	if( elNuevo != NULL ){
		list_add( tabla_segmentos, elNuevo );
	}

	//log_info( logger, "Termine de crear el segmento, ahora hay %d", list_size( tabla_segmentos ) );
	return elNuevo;

}


Segmento * crearSegmentoFistFit( t_list * huequitos, uint32_t tamanio ) {

	int i = 0;
	Segmento * huequito = malloc( sizeof( Segmento ) );
	for( i = 0; i <  list_size( huequitos ); i++ ) {
		huequito = (Segmento*) list_get( huequitos, i );
		if( ( huequito->finReal - huequito->inicioReal ) >= tamanio ){
			//IUJU hay espacio :D
			Segmento * elNuevo = new_Segmento( huequito->inicioReal, huequito->inicioReal + tamanio );
			return elNuevo;
		}
	}

	free(huequito);

	return NULL;

}

Segmento * crearSegmentoWorstFit( t_list * huequitos, uint32_t tamanio ) {

	int i = 0, tamanioMax = 0, tamanioHuequito = 0;
	Segmento * nuevoSegmento = NULL;

	for (i = 0; i < list_size( huequitos ); i++) {
		Segmento * huequito = (Segmento*) list_get(huequitos, i);
		tamanioHuequito = huequito->finReal - huequito->inicioReal + 1;
		if ( tamanioHuequito >= tamanioMax && tamanioHuequito >= tamanio ) {
			nuevoSegmento = huequito;
			tamanioMax = tamanioHuequito;
		}
	}

	if( tamanioMax >= tamanio ){
		return new_Segmento( nuevoSegmento->inicioReal,  nuevoSegmento->inicioReal + tamanio );
	}else{
		return NULL;
	}

}





bool segmentoEsAnterior( void * seg1, void * seg2 ) {
	Segmento * segmento1 = (Segmento * ) seg1;
	Segmento * segmento2 = (Segmento * ) seg2;
	return segmento1->inicioReal < segmento2->inicioReal;
}



/*
 * Ordena la tabla de segmentos segun su inicio
 *	TODO usar semaforo
 */
void ordenarTablaSegmentos(){
	list_sort( tabla_segmentos , &segmentoEsAnterior );
}







/*
 * Crea una lista de "Segmentos" indicando el inicio y fin de cada espacio libre.
 * Esta lista en si lo que hace es crear todos los segmentos posibles ocupando el mayor espacio posible
 * En fin devuelve los huequitos vacios
 *
 */
//TODO usar semaforo
//TODO validar, son necesarios los +1 ?
t_list * crearListaEspacioDisponible() {

	ordenarTablaSegmentos();

	t_list * lista = list_create();

	if( list_size( tabla_segmentos ) == 0 ){

		Segmento * segmento = new_Segmento( 0, memoria_size );
		list_add( lista, segmento );

	}else{

		Segmento * primerSegmento = (Segmento *) list_get( tabla_segmentos, 0 );

		if( primerSegmento->inicioReal != 0 ) {
			Segmento * segmentoInicial = new_Segmento( 0, primerSegmento->inicioReal - 1 );
			list_add( lista, segmentoInicial );
		}


		int i = 0;
		for( i = 0; i < list_size( tabla_segmentos ) - 1 ; i++ ){

			Segmento * segmento1 = (Segmento *) list_get( tabla_segmentos, i );
			Segmento * segmento2 = (Segmento *) list_get( tabla_segmentos, i + 1 );

			 //Si el espacio entre esos 2 segmentos no es contino no es contiguo...
			if( segmento1->finReal != segmento2->inicioReal - 1 ) {
				Segmento * segmentoIntermedio = new_Segmento( segmento1->finReal + 1 , segmento2->inicioReal );
				list_add( lista, segmentoIntermedio );
			}

		}

		Segmento * ultimoSegmento = (Segmento *) list_get( tabla_segmentos,  list_size( tabla_segmentos ) - 1 );
		if( ultimoSegmento->finReal != memoria_size ) {
			Segmento * segmentoFinal = new_Segmento( ultimoSegmento->finReal + 1, memoria_size );
			list_add( lista, segmentoFinal );
		}

	}

	//printf("Mostrando tabla de segmentos y huequitos");
	//printSegmentos( tabla_segmentos );
	//printSegmentos( lista );

	//log_info( logger, "Devuelvo una lista con %d item/s", list_size( lista ) );
	return lista;

}

void borrarSegmento( Segmento * segmentoABorrar ) {
	int i = 0;
	for( i = 0; i < list_size( tabla_segmentos ); i++ ) {
		Segmento * segmento = (Segmento *) list_get( tabla_segmentos, i );
		if( segmento->id == segmentoABorrar->id ) {
			list_remove( tabla_segmentos , i);
			free( segmentoABorrar );
			log_info( logger, "Se borro el segmento correctamente" );
			return;
		}
	}
	log_error( logger, "No pudo borrarse el segmento" );
}

uint32_t memoriaOcupada() {
	int i = 0, sumador = 0;
	for (i = 0; i < list_size(tabla_segmentos); i++) {
		Segmento * segmento = (Segmento *) list_get(tabla_segmentos, i);
		sumador += segmento->finReal - segmento->inicioReal + 1 ;
	}
	return sumador;
}

uint32_t memoriaLibre(){
	return memoria_size - memoriaOcupada();
}




void compactar(){
		ordenarTablaSegmentos();

		int tamanio;
		Segmento * primerSegmento = (Segmento *) list_get (tabla_segmentos, 0);
		if(primerSegmento-> inicioReal != 0){
			tamanio = tamanioSegmento(primerSegmento);
			moverSegmento(primerSegmento, 0 );
		}
		int i = 0;
		for(i=0; i < list_size(tabla_segmentos) - 1; i++){
			int tamanio;
			Segmento * segmentoMovido = (Segmento *) list_get (tabla_segmentos,i);
			Segmento * segmentoAmover = (Segmento *) list_get( tabla_segmentos, i + 1 );
		    tamanio = tamanioSegmento(segmentoAmover);
			moverSegmento(segmentoAmover, segmentoMovido->finReal + 1 );
		}
		log_info(logger, "Se ha compactado correctamente");
		printTodosSegmentos();
		return ;
}


void moverSegmento(Segmento * segmento, uint32_t posicion) {

	uint32_t tamanio = tamanioSegmento( segmento );
	uint32_t nuevoInicio = posicion;
	uint32_t nuevoFin = posicion + tamanio;

	//TODO
	//memcpy( memoria + nuevoInicio, segmento->inicioReal, tamanio );

	segmento->inicioReal = nuevoInicio;
	segmento->finReal = nuevoFin;
	return;

}

int tamanioSegmento(Segmento * segmento) {
	return (segmento->finReal - segmento->inicioReal);
}





















