#include "umv.h"
#include "stack.h"
#include "ejecucion.h"

#include "commons/log.h"

#include <stdio.h>

extern pcb_t * PCB_enEjecucion;
extern t_log * logger;


const char lineasCodigo[][25] = {
	"variables a,b",
	"a = 2",
	"b = 4",
	"c = a * b",
	"imprimir c"
};



void testStack() {

	Stack * stack = malloc(sizeof(Stack));
	stack->dataLength = 50;
	stack->offset = 30;


	char * A = "A";
	printf( "Se creo 'A' en la posicion: %d \n",  apilarVariable( stack, A ) ) ;
	printf( "Si pido la posicion de 'A' da: %d\n", obtenerOffsetVarible( stack, A ) );
	modificarVariable( stack, obtenerOffsetVarible( stack, A ) , 5 );
	printf( "Le guardo '5' en 'A' y cuando la lee me dice: %d \n", obtenerValor( stack, obtenerOffsetVarible( stack, A ) ) );

	char * J = "J";
	printf( "\nSe creo 'J' en la posicion: %d \n",  apilarVariable( stack, J ) ) ;
	printf( "Si pido la posicion de 'j' da: %d\n", obtenerOffsetVarible( stack, J ) );
	modificarVariable( stack, obtenerOffsetVarible( stack, J ) , 8 );
	printf( "Le guardo '8' en 'J' y cuando la lee me dice: %d \n", obtenerValor( stack, obtenerOffsetVarible( stack, J ) ) );


	char * P = "P";
	printf( "\nSe creo 'J' en la posicion: %d \n",  apilarVariable( stack, P ) ) ;
	printf( "Si pido la posicion de 'P' da: %d\n", obtenerOffsetVarible( stack, P ) );
	modificarVariable( stack, obtenerOffsetVarible( stack, P ) , 7 );
	printf( "Le guardo '7' en 'P' y cuando la lee me dice: %d \n", obtenerValor( stack, obtenerOffsetVarible( stack, P ) ) );


}


int ejecutarPrueba() {

	printf("llegua hasta aca\n\n");
	PCB_enEjecucion->codeSegment = 45;
	PCB_enEjecucion->contextSize = 45;
	PCB_enEjecucion->etiquetaIndex = 2;
	PCB_enEjecucion->id = 4;
	PCB_enEjecucion->lastErrorCode = 0;
	PCB_enEjecucion->prioridad = 2;
	PCB_enEjecucion->programCounter = 432;
	PCB_enEjecucion->programaSocket = 32423;
	//PCB_enEjecucion->stackIndex = 234;

	ejecutar();
	//testStack();
	return 1;

}





