#include <sys/socket.h>
#include<string.h>

#include "ejecucion.h"
#include "kernel.h"

#include "commons/pcb.h"
#include "commons/sockets.h"


#define BUFF_SIZE 1024

extern int conexionKernel;
extern t_log * logger;
extern pcb_t PCB_enEjecucion;

extern uint32_t quantumPorEjecucion;
extern uint32_t retardo;



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
	socket_scObtenerValor sObtenerValor;
	sObtenerValor.header.code = 'o';
	sObtenerValor.header.size = sizeof(socket_scObtenerValor);
	strcpy( sObtenerValor.identificador, variable );

	if( send(conexionKernel, &sObtenerValor, sizeof(socket_scObtenerValor), 0) < 0 )
		return false;

	if( recv(conexionKernel, &sObtenerValor, sizeof(socket_scObtenerValor), MSG_WAITALL) != sizeof(socket_scObtenerValor) )
		return false;

	return sObtenerValor.valor;
}

bool enviarAKernelNuevoValorVariableCompartida(t_nombre_compartida variable, t_valor_variable valor)
{
	socket_scGrabarValor sGrabarValor;
	sGrabarValor.header.code = 'g';
	sGrabarValor.header.size = sizeof(socket_scGrabarValor);
	sGrabarValor.valor = valor;
	strcpy( sGrabarValor.identificador, variable );

	if( send(conexionKernel, &sGrabarValor, sizeof(socket_scGrabarValor), 0) < 0 )
		return false;

	return true;
}

bool enviarAKernelImprimir( t_valor_variable valor )
{
	socket_imprimirTexto mensaje;
	mensaje.header.code = 'k';
	mensaje.header.size = sizeof( socket_imprimirTexto );
	mensaje.programaSocket = PCB_enEjecucion.programaSocket;
	sprintf(mensaje.texto, "%d", valor);

	if( send(conexionKernel, &mensaje, sizeof(socket_imprimirTexto), 0) < 0 )
		return false;

	return true;
}

bool enviarAKernelImprimirTexto( char * texto )
{
	socket_imprimirTexto mensaje;
	mensaje.header.code = 'k';
	mensaje.header.size = sizeof( socket_imprimirTexto );
	mensaje.programaSocket = PCB_enEjecucion.programaSocket;
	strcpy( mensaje.texto, texto );

	if( send(conexionKernel, &mensaje, sizeof(socket_imprimirTexto), 0) < 0 )
			return false;

	return true;
}

bool enviarAKernelEntradaSalida(t_nombre_dispositivo dispositivo, int tiempo)
{
	socket_scIO io;
	io.header.code = 'i';
	io.header.size = sizeof(socket_scIO);
	io.unidades = tiempo;
	strcpy(io.identificador, dispositivo);

	if( send(conexionKernel, &io, sizeof(socket_scIO), 0) < 0 )
		return false;

	if( enviarPCB() < 0 )
		return false;

	return true;
}

bool enviarAKernelSignal(t_nombre_semaforo identificador_semaforo)
{
	socket_scSignal sSignal;
	sSignal.header.code = 's';
	sSignal.header.size = sizeof(socket_scSignal);
	strcpy(sSignal.identificador, identificador_semaforo );

	if( send(conexionKernel, &sSignal, sizeof(socket_scSignal), 0) < 0 )
		return false;

	return true;
}

bool enviarAKernelWait(t_nombre_semaforo identificador_semaforo)
{
	socket_scWait sWait;
	sWait.header.code = 'w';
	sWait.header.size = sizeof(socket_scWait);
	strcpy(sWait.identificador, identificador_semaforo );

	if( send(conexionKernel, &sWait, sizeof(socket_scWait), 0) < 0 )
		return false;

	socket_respuesta res;

	if( recv(socket, &res, sizeof(socket_respuesta), MSG_WAITALL) != sizeof(socket_respuesta) )
		return false;

	if(!res.valor)
	{
		//Enviar pcb y detener ejecucion

		if( enviarPCB() < 0 )
			return false;

		//Detener ejecucion ???
	}

	return true;
}



/****************************************************/


int enviarPCB()
{
	socket_pcb spcb;
	spcb.header.size = sizeof(socket_pcb);
	spcb.pcb = PCB_enEjecucion;

	return send(conexionKernel, &spcb, sizeof(socket_pcb), 0);
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

