#include "stack.h"

#include "commons/pcb.h"
#include "commons/log.h"

#include <stdio.h>

extern pcb_t * PCB_enEjecucion;
extern t_log * logger;


/*
 *	Apila una variable en el stack dado, y devuelve un "puntero" a esa variable
 */
//TODO, verificar si la variable no existe
uint32_t apilarVariable( Stack * stack, char * identificador ) {
	stack->data[stack->dataLength] = (char) *identificador;
	stack->dataLength += 5;
	return stack->dataLength - 5;
}



/*
 * Obtiene la posicion de memoria de una variable
 */
//TODO determinar que hacer si la variable no esta definida
uint32_t obtenerOffsetVarible( Stack * stack, char * variable ) {
	int i = 0;
	for( i = 0; i < stack->dataLength ; i +=5 ) {
		if( stack->data[i] == *variable ){
			return i;
		}
	}
	log_error( logger, "Se solicito la posicion de memoria de una variable inexistente" );
	return 0;
	//return apilarVariable( stack, variable );
}




//TODO ver segmentation fault
uint32_t obtenerValor( Stack * stack, uint32_t pos ) {
	uint32_t valor;
	memcpy( &valor , &stack->data + pos , 4 );
	return valor;
}


//TODO verificar segmentation fault
void modificarVariable( Stack * stack, uint32_t pos, uint32_t valor ) {
	memcpy( &stack->data + pos , &valor, 4 );
}





void apilarFuncionConRetorno( Stack * stack, uint32_t context_init, uint32_t programCounter, uint32_t variableRetorno ) {

}


void apilarFuncionSinRetorno( Stack * stack, uint32_t context_init, uint32_t programCounter ) {

}




















