#include <sys/socket.h>
#include<string.h>

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


int orden_ejecucion()
{
	return responder_orden_ejecucion( ejecutar () );
}


//recibe un pcb despues de ser ejecutado y se lo re envia al kernel
int responder_orden_ejecucion()
{
	return 1;
}



/****************** SYSCALLS ************************/

uint32_t solcitarVariableCompartidaAKernel(t_nombre_compartida variable)
{
	socket_scObtenerValor mensaje;
	strcpy( (void * ) &mensaje.identificador, variable );
	enviarYRecibirPaquete( conexionKernel, &mensaje, sizeof( socket_scObtenerValor ) , 0, 'o', 'a', logger );
	return 1;
}

int enviarAKernelNuevoValorVariableCompartida(t_nombre_compartida variable, t_valor_variable valor)
{
	socket_scGrabarValor mensaje;
	mensaje.valor = valor;
	strcpy( (void * ) &mensaje.identificador, variable );
	enviarYRecibirPaquete( conexionKernel, &mensaje, sizeof( socket_scGrabarValor ) , 0, 'g', 'a', logger );
	return 1;
}

int enviarAKernelImprimir( t_valor_variable valor )
{
	socket_imprimir mensaje;
	mensaje.valor = valor;
	return enviarPaquete( conexionKernel, &mensaje, sizeof( socket_imprimir ) , 'n', logger ) && enviarPCB();
}

int enviarAKernelImprimirTexto( char * texto )
{
	socket_imprimirTexto mensaje;
	strcpy( (void * ) &mensaje.texto, texto );
	return enviarPaquete( conexionKernel, &mensaje, sizeof( socket_imprimirTexto ) , 'k', logger ) && enviarPCB();
}

int enviarAKernelEntradaSalida(t_nombre_dispositivo dispositivo, int tiempo)
{
	socket_scIO mensaje;
	mensaje.unidades = tiempo;
	strcpy( (void *) &mensaje.identificador , dispositivo );
	return enviarPaquete( conexionKernel, &mensaje, sizeof( socket_scIO ) , 'i', logger ) && enviarPCB();
}

int enviarAKernelSignal(t_nombre_semaforo identificador_semaforo)
{
	socket_scSignal mensaje;
	strcpy( (void * ) &mensaje.identificador, identificador_semaforo );
	enviarPaquete( conexionKernel, &mensaje, sizeof( socket_scSignal ), 's' , logger);
	return 1;
}

int enviarAKernelWait(t_nombre_semaforo identificador_semaforo)
{
	socket_scWait mensaje;
	strcpy( (void * ) &mensaje.identificador, identificador_semaforo );

	return 1;
}
/****************************************************/




int enviarPCB()
{
	return enviarPaquete( conexionKernel, &PCB_enEjecucion, sizeof( socket_pcb ) , 'k', logger );
}

int enviarHandshake()
{

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
int procesarMensajesKernel( int socket, socket_header header )
{



	return 1;

}


//TODO se podria reusar lo de la umv ??
int recibirYProcesarMensajesKernel()
{
	return 1;
}

