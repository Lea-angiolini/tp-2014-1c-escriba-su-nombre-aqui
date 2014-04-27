#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "Consola.h"
#include "Segmento.h"

#include "commons/log.h"
#include "commons/string.h"
#include "commons/collections/list.h"


char comandosBuffer[200];
extern t_log * logger;
extern t_list * tabla_segmentos;


void * iniciarConsola( void * params ){

	printf( "Consola de UMV, escriba-su-nombre-aqui\n");
	printf( "All rights reserved. 2014\n\n");

	while( 1 ){

		memset( &comandosBuffer, 0x0000, sizeof( comandosBuffer ) );
		printf( ">>> " );
		gets( &comandosBuffer );
		if( parsearComando( &comandosBuffer ) == 0){
			break;
		}

	}

	log_info( logger, "\n\nFinalizando la consola ..." );
	return NULL;
}


int parsearComando( char * comando ) {

	if ( string_starts_with( comando, "holi" ) ){
		printf("El comando es holi\n\n");
	}

	if ( string_starts_with( comando, "salir" ) ){
		return 0;
	}

	return 1;
}






void printTodosSegmentos() {
	printSegmentos( tabla_segmentos );
}

void printSegmentos( t_list * segmentos ) {
	printSegmentosHeaders();
		int i = 0;
		for( i = 0; i < list_size( segmentos ) ; i++ ){
			Segmento * segmento = (Segmento *) list_get( segmentos, i );
			printSegmento( segmento );
		}
}

void printSegmentosHeaders(){
	printf("\n\n");
	printf("\t\tInicio Real\tFin Real\tTamaño\n\n");
}

void printSegmento( Segmento * segmento ) {
	printf(">>>\t\t%d\t\t%d\t\t%d\n", segmento->inicioReal, segmento->finReal, segmento->finReal - segmento->inicioReal + 1 );
}
































