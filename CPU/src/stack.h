#include <stdint.h>

#ifndef STACK_H_
#define STACK_H_


/*
 * Esta estructura tiene una porcion del stack. Puede ser cualquier porcion.
 * Lo importante es que una vez que inicia el quantum, se solicita el stack
 *
 * desde: stackCursor - contextSize = offset
 * hasta:  stackCursor
 *
 * Al terminar el quantum se envia a la UMV para actualizar la informacion.
 * El único problema es si hay un return que probablemente se tenga que pedir
 * mas info de la que hay aca.
 *
 *
 */

//TODO crear el tamaño total para verificar segmentation fault
typedef struct {

	uint32_t offset;
	uint32_t dataLength;
	char data[1000];

} __attribute__((packed)) Stack;

Stack * new_Stack();

uint32_t apilarVariable( Stack * stack, char identificador );
uint32_t obtenerOffsetVarible( Stack * stack, char variable );
uint32_t obtenerValor( Stack * stack, uint32_t pos );
void modificarVariable( Stack * stack, uint32_t pos, uint32_t valor );


#endif /* STACK_H_ */






