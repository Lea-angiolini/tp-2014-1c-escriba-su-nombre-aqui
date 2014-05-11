#include <sys/socket.h>

#include "ejecucion.h"
#include "kernel.h"

#include "commons/pcb.h"
#include "commons/sockets.h"


#define BUFF_SIZE 1024

extern int conexionKernel;
extern t_log * logger;
extern pcb_t * PCB_enEjecucion;

extern int quantumPorEjecucion;
extern int retardo;



/*
 *	Instrucciones que el CPU puede recibir del kernel
 *
 *
 */


int orden_ejecucion() {


	return responder_orden_ejecucion( ejecutar () );

}


//recibe un pcb despues de ser ejecutado y se lo re envia al kernel
int responder_orden_ejecucion() {
	return 1;
}






uint32_t solcitarVariableCompartidaAKernel() {
	return 1;
}


int enviarAKernelNuevoValorVariableCompartida() {
	return 1;
}


int enviarAKernelImprimir() {
	return 1;
}

int enviarAKernelImprimirTexto() {
	return 1;
}

int enviarAKernelEntradaSalida() {
	return 1;
}




int enviarHandshake() {

	socket_header cod;
	cod.code = 'h';
	cod.size = sizeof( socket_header );

	if ( send( conexionKernel, &cod, cod.size, 0 ) < 0 ) {
		return -1;
	}

	socket_cpucfg * cpucfg = (socket_cpucfg *) recibirPaquete( conexionKernel, 0, 'h', logger );
	if( cpucfg == NULL ) {
		log_error(logger, "No se pudo leer el la configuracion que envio el Kernel | CPU/src/CPU.c -> crearConexionKernel");
		return -1;
	}

	quantumPorEjecucion = cpucfg->quantum;
	retardo = cpucfg->retardo;
	log_info( logger, "El quantum que envio el kernel es: %d y el retardo: %d", quantumPorEjecucion, retardo );

	return 1;
}



//TODO definir codigos de operacion
int procesarMensajesKernel( int socket, socket_header header ) {



	return 1;

}


//TODO se podria reusar lo de la umv ??
int recibirYProcesarMensajesKernel() {
	return 1;
}

