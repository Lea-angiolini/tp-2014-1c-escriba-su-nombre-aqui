#include "umv.h"
#include "stack.h"
#include "ejecucion.h"

#include "commons/log.h"

#include <string.h>
#include <stdio.h>


extern Stack stackCache;


void guardarCosasEnStack() {

	printf( "Se creo 'A' en la posicion: %d \n",  apilarVariable( 'A' ) );
	printf( "Se creo 'J' en la posicion: %d \n",  apilarVariable( 'J' ) );
	apilarVariable( 'Q' );
	apilarVariable( 'R' );
	printf( "Se creo 'P' en la posicion: %d \n",  apilarVariable( 'P' ) ) ;

	printf( "El offset de A es: %d\n", obtenerOffsetVarible( 'A' ) );
	printf( "El offset de J es: %d\n", obtenerOffsetVarible( 'J' ) );
	printf( "El offset de Q es: %d\n", obtenerOffsetVarible( 'Q' ) );

	printf( "Guardo el valor 84 en 'A' \n"); modificarVariable( obtenerOffsetVarible( 'A' ), 84 );
	printf( "Guardo el valor  7 en 'J' \n"); modificarVariable( obtenerOffsetVarible( 'J' ), 7 );
	printf( "Guardo el valor 19 en 'P' \n"); modificarVariable( obtenerOffsetVarible( 'P' ), 19 );

}


void leerVariablesDelStack()
{

	printf( "Si pido la posicion de 'A' da: %d y su valor es: %d \n", obtenerOffsetVarible( 'A' ), obtenerValor( obtenerOffsetVarible( 'A' ) ) );
	printf( "Si pido la posicion de 'J' da: %d y su valor es: %d \n", obtenerOffsetVarible( 'J' ), obtenerValor( obtenerOffsetVarible( 'J' ) ) );
	printf( "Si pido la posicion de 'P' da: %d y su valor es: %d \n", obtenerOffsetVarible( 'P' ), obtenerValor( obtenerOffsetVarible( 'P' ) ) );

}


bool ejecutarPrueba() {


	printf("Guardando cosas cualquieras en el stack\n");
	guardarCosasEnStack();
	printf("Enviando el stack actual a la UMV...\n\n");
	guardarStack();
	printf("Limpiando el stack con un par de 0s\n\n");

	int i = 0;
	for( i = 0; i < 100; i++){
		stackCache.data[ i ] = '0';
	}

	printf("Obteniendo el stack actual\n\n");
	obtenerContextStack();
	leerVariablesDelStack();
	printf("Finalizo sin segmentation fault");
	return true;

}





