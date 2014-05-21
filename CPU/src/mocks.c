#include "umv.h"
#include "stack.h"
#include "ejecucion.h"

#include "commons/log.h"

#include <string.h>
#include <stdio.h>

extern pcb_t PCB_enEjecucion;
extern t_log * logger;
extern Stack * stackCache;


const char lineasCodigo[][25] = {
	"variables a,b",
	"a = 2",
	"b = 4",
	"c = a * b",
	"imprimir c"
};



void guardarCosasEnStack() {


	char A = 'A';
	char J = 'J';
	char P = 'P';

	printf( "Se creo 'A' en la posicion: %d \n",  apilarVariable( stackCache, A ) );
	printf( "Se creo 'J' en la posicion: %d \n",  apilarVariable( stackCache, J ) );
	apilarVariable( stackCache, 'Q' );
	apilarVariable( stackCache, 'R' );
	printf( "Se creo 'P' en la posicion: %d \n",  apilarVariable( stackCache, P ) ) ;

	printf( "Guardo el valor 8 en 'A' \n");
	modificarVariable( stackCache, obtenerOffsetVarible( stackCache, A ) , 8 );
	printf( "Guardo el valor 7 en 'J' \n");
	modificarVariable( stackCache, obtenerOffsetVarible( stackCache, J ) , 7 );
	printf( "Guardo el valor 1 en 'P' \n");
	modificarVariable( stackCache, obtenerOffsetVarible( stackCache, P ) , 1 );

}


void leerVariablesDelStack()
{

	char A = 'A';
	char J = 'J';
	char P = 'P';

	printf("\n\nAhora voy a leer 'A'\n");
	printf( "Si pido la posicion de 'A' da: %d y su valor es: %d \n", obtenerOffsetVarible( stackCache, A ), obtenerValor( stackCache, obtenerOffsetVarible( stackCache, A ) ) );
	printf( "Si pido la posicion de 'J' da: %d y su valor es: %d \n", obtenerOffsetVarible( stackCache, J ), obtenerValor( stackCache, obtenerOffsetVarible( stackCache, J ) ) );
	printf( "Si pido la posicion de 'P' da: %d y su valor es: %d \n", obtenerOffsetVarible( stackCache, P ), obtenerValor( stackCache, obtenerOffsetVarible( stackCache, P ) ) );


}


bool ejecutarPrueba() {

	PCB_enEjecucion.codeSegment = 45;
	PCB_enEjecucion.contextSize = 45;
	PCB_enEjecucion.etiquetaIndex = 2;
	PCB_enEjecucion.id = 4;
	PCB_enEjecucion.lastErrorCode = 0;
	PCB_enEjecucion.prioridad = 2;
	PCB_enEjecucion.programCounter = 432;
	PCB_enEjecucion.programaSocket = 32423;


	printf("Ejecutando........\n\n");
	printf("Guardando cosas cualquieras en el stack\n");
	guardarCosasEnStack();
	printf("Enviando el stack actual a la UMV...\n\n");
	guardarStack();
	printf("Limpiando el stack con un par de 0s\n\n");
	memset( stackCache->data, 0, 100);
	printf("Obteniendo el stack actual\n\n");
	obtenerContextStack();
	leerVariablesDelStack();


	return true;

}





