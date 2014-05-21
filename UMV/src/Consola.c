#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "Consola.h"
#include "Segmento.h"

#include "commons/log.h"
#include "commons/string.h"
#include "commons/collections/list.h"
#include "memoria.h"


char comandosBuffer[200];
extern t_log * logger;
extern t_list * tabla_segmentos;
extern uint32_t retardoUMV;
extern char * modoActualCreacionSegmentos;

void * iniciarConsola( void * params ){

	printf( "Consola de UMV, escriba-su-nombre-aqui\n");
	printf( "All rights reserved. 2014\n\n");

	while( 1 ){

		char comando;
		printf("¿Que operacion desea realizar?\n");
		printf("Ingrese por teclado la letra de la operacion a realizar");
		printf("a - Operaciones con segmentos\n");
		printf("b - Modificar retardo ante una solicitud\n");
		printf("c - Modificar algoritmo de modificacion de segmentos\n");
		printf("d - Compactar tabla de segmentos\n");
		printf("e - Generar dump\n");
		printf("f - Finalizar consola\n");

		comando = getchar();
		switch( comando ){
		case 'a': operacionesConSegmentos();
				break;
		case 'b': modificarRetardoUMV();
				break;
		case 'c': modificarAlgoCreacionSegmentos();
				break;
		case 'd': compactar();
				break;
		case 'e': generarDump();
				break;
		case 'f': break;

		default : log_error( logger, "El comando ingresado no es valido" );
					break;
		}
		if( comando == 'f')
			break;

		}

	log_info( logger, "\n\nFinalizando la consola ..." );
	return NULL;
}


void operacionesConSegmentos(){

}

void modificarRetardoUMV(){
	uint32_t retardo;

	printf("Especifique el retardo que quiere modificar en milisegundos \n");
	scanf("%d", &retardo);
	retardoUMV = retardo;
}

void modificarAlgoCreacionSegmentos(){

	char modo;

	printf("Indique el modo con el que quiere crear segmentos\n");
	printf("a - Worst-Fit \n b - First-Fit");
	scanf("%c", &modo);

	switch( modo ){
	case 'a': if( string_starts_with( modoActualCreacionSegmentos, "WORSTFIT"))
				break;
			modoActualCreacionSegmentos = "WORSTFIT";
				break;

	case 'b': if( string_starts_with( modoActualCreacionSegmentos, "FIRSTFIT"))
					break;
			modoActualCreacionSegmentos = "FIRSTFIT";
					break;
	default: log_error( logger, "El comando ingresado para modificar el modo con el que quiere crear segmentos es invalido");
	}
}


void generarDump(){

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
































