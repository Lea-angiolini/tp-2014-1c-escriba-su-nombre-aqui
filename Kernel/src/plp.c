#include "plp.h"
#include <stdio.h>
#include <stdbool.h>

void *IniciarPlp(void *arg)
{
	printf("[PLP] Thread iniciado");
	while(true)
	{
	//socket entra
	//listen
	/*
	do while {
	accept
		if( puedeMoverNewAReady() )
		{
			do {
				//New->Ready
				list_add(readyQueue, queue_pop(newQueue));
				multiprogramacion++;
			} while( puedeMoverNewAReady() );

			list_sort(readyQueue, sjnAlgorithm);
		}
	}


	*/

	//recv && recvfile
	//ansisop preprocesador
	//request memory plp->umv response request memory
	//contruir pcb_t pcb;
	//queue_push(newQueue, pcb);
	}
	return NULL;
}
/*
bool puedeMoverNewAReady()
{
	if(multiprogramacion == config(multiprogramacion) || queue_is_empty(newQueue) )
		return false;

	return true;
}*/

unsigned int calcularPrioridad(unsigned int e, unsigned int f, unsigned int t)
{
	return 5*e+3*f+t;
}

/*
bool sjnAlgorithm(pcb_t *a, pcb_t *b)
{
	if(a->prioridad < b->prioridad)
		return false;

	return true;
}*/
