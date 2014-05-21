#include "stack.h"

#include "commons/pcb.h"
#include "commons/log.h"


extern pcb_t PCB_enEjecucion;
extern t_log * logger;


Stack * new_Stack()
{
	Stack * stack = (Stack *) malloc( sizeof( Stack ) );
	stack->dataLength = 0;
	stack->offset = 0;
	return stack;
}


/*
 *	Apila una variable en el stack dado, y devuelve un "puntero" a esa variable
 */
//TODO, verificar si la variable no existe
uint32_t apilarVariable( Stack * stack, char identificador )
{
	stack->data[stack->dataLength] = identificador;
	stack->dataLength += 5;
	return stack->dataLength - 4;
}


/*
 * Obtiene la posicion de memoria de una variable
 */
//TODO determinar que hacer si la variable no esta definida
uint32_t obtenerOffsetVarible( Stack * stack, char variable )
{
	int i = 0;
	//printf("El dataLength = %d, i = %d \n" , stack->dataLength, i );
	for( i = 0; i < stack->dataLength ; i +=1 ) {
		//printf( "Evaluando variable %c comprado con: %c, son iguales ? %d \n", stack->data[i], variable, stack->data[i] == variable );
		//printf("El dataLength = %d, i = %d \n" , stack->dataLength, i );
		if( stack->data[i] == variable ) {
			return i+1;
		}
	}
	log_error( logger, "Se solicito la posicion de memoria de una variable inexistente" );
	return 0;
}


//TODO ver segmentation fault
uint32_t obtenerValor( Stack * stack, uint32_t pos )
{
	uint32_t valor;
	memcpy( &valor , stack->data + pos , 4 );
	return valor;
}


//TODO verificar segmentation fault
void modificarVariable( Stack * stack, uint32_t pos, uint32_t valor )
{
	memcpy( stack->data + pos , &valor, 4 );
}



void apilarFuncionConRetorno( Stack * stack, uint32_t context_init, uint32_t programCounter, uint32_t variableRetorno )
{

}


void apilarFuncionSinRetorno( Stack * stack, uint32_t context_init, uint32_t programCounter )
{

}


















