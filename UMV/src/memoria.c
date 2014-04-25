#include <stdint.h>
#include <stdlib.h>

#include "Segmento.h"
#include "memoria.h"

#include "commons/collections/list.h"
#include "commons/log.h"


extern t_list * tabla_segmentos;
extern uint32_t memoria_size;
extern t_log * logger;

uint32_t crearSegmento( uint32_t tamanio ) {

	log_info( logger, "Creando segmento, ahora hay %d", list_size( tabla_segmentos ) );
	//crearListaEspacioDisponible();
	Segmento * elNuevo = NULL;

	if( list_size( tabla_segmentos ) == 0 && memoria_size > tamanio ){

		elNuevo = new_Segmento( 0, tamanio );
		list_add( tabla_segmentos, elNuevo );

	}else{

		t_list * huequitos = crearListaEspacioDisponible();
		crearSegmentoFistFit( huequitos,  tamanio ) ;
		list_destroy( huequitos );

	}

	log_info( logger, "Termine de crear el segmento, ahora hay %d", list_size( tabla_segmentos ) );
	return 0;

}


uint32_t crearSegmentoFistFit( t_list * huequitos, uint32_t tamanio ) {


	int i = 0, creado = 0;
	Segmento * huequito = malloc( sizeof( Segmento ) );
	for( i = 0; i <  list_size( huequitos ); i++ ) {

		huequito = (Segmento*) list_get( huequitos, i );

		if( ( huequito->finVirtual - huequito->inicioReal ) >= tamanio ){

			//IUJU hay espacio :D
			Segmento * elNuevo = new_Segmento( huequito->inicioVirtual, huequito->finVirtual + tamanio );
			list_add( tabla_segmentos, elNuevo );
			creado = 1;
			break;

		}

	}

	if( !creado ){
		log_info( logger, "No se pudo hubicar un espacio en memoria, compactando!! Mentira, todavia no se compactar =( " );
	}


	free(huequito);

	return 0;

}


uint32_t crearSegmentoWorstFit( t_list * huequitos, uint32_t tamanio ){

	return 0;

}



/*
 * Ordena la tabla de segmentos segun su inicio
 *
 */
void ordenarTablaSegmentos(){

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

		if( primerSegmento == NULL ){
			log_info( logger, "La concha de tu madre " );
		}

		if( primerSegmento->inicioReal != 0 ) {
			Segmento * segmentoInicial = new_Segmento( 0, primerSegmento->inicioReal - 1 );
			list_add( lista, segmentoInicial );
		}


		int i = 0;
		for( i = 0; i < list_size( tabla_segmentos ) - 1 ; i++ ){

			Segmento * segmento1 = (Segmento *) list_get( tabla_segmentos, i );
			Segmento * segmento2 = (Segmento *) list_get( tabla_segmentos, i + 1 );
			Segmento * segmentoIntermedio = new_Segmento( segmento1->finReal + 1 , segmento2->inicioReal - 1 );
			list_add( lista, segmentoIntermedio );

		}

		Segmento * ultimoSegmento = (Segmento *) list_get( tabla_segmentos,  list_size( tabla_segmentos ) - 1 );

		if( ultimoSegmento->finReal != memoria_size ) {
			Segmento * segmentoFinal = new_Segmento( ultimoSegmento->finReal + 1, memoria_size );
			list_add( lista, segmentoFinal );
		}

	}

	log_info( logger, "Devuelvo una lista con %d item/s", list_size( lista ) );
	return lista;

}










int compactar(){

	return 1;

}

























