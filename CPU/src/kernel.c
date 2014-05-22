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
	if( enviarHeaderCodeaKernel('o') < 0 )
		return false;

	socket_scObtenerValor sObtenerValor;
	strcpy( sObtenerValor.identificador, variable );

	if( send(conexionKernel, &sObtenerValor, sizeof(socket_scObtenerValor), 0) < 0 )
		return false;

	if( recv(conexionKernel, &sObtenerValor, sizeof(socket_scObtenerValor), MSG_WAITALL) != sizeof(socket_scObtenerValor) )
		return false;

	return sObtenerValor.valor;
}

bool enviarAKernelNuevoValorVariableCompartida(t_nombre_compartida variable, t_valor_variable valor)
{
	if( enviarHeaderCodeaKernel('g') < 0 )
		return false;

	socket_scGrabarValor sGrabarValor;
	sGrabarValor.valor = valor;
	strcpy( sGrabarValor.identificador, variable );

	if( send(conexionKernel, &sGrabarValor, sizeof(socket_scGrabarValor), 0) < 0 )
		return false;

	return true;
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
	strcpy( mensaje.texto, texto );
	return enviarPaquete( conexionKernel, &mensaje, sizeof( socket_imprimirTexto ) , 'k', logger ) && enviarPCB();
}

bool enviarAKernelEntradaSalida(t_nombre_dispositivo dispositivo, int tiempo)
{
	if( enviarHeaderCodeaKernel('i') < 0 )
		return false;

	socket_scIO io;
	io.unidades = tiempo;
	strcpy(io.identificador, dispositivo);

	if( send(conexionKernel, &io, sizeof(socket_scIO), 0) < 0 )
		return false;

	if( enviarPCB() < 0 )
		return false;

	return true;
	//return enviarPaquete( conexionKernel, &mensaje, sizeof( socket_scIO ) , 'i', logger ) && enviarPCB();
}

bool enviarAKernelSignal(t_nombre_semaforo identificador_semaforo)
{
	if( enviarHeaderCodeaKernel('s') < 0 )
		return false;

	socket_scSignal sSignal;
	strcpy(sSignal.identificador, identificador_semaforo );

	if( send(conexionKernel, &sSignal, sizeof(socket_scSignal), 0) < 0 )
		return false;

	return true;
}

bool enviarAKernelWait(t_nombre_semaforo identificador_semaforo)
{
	if( enviarHeaderCodeaKernel('w') < 0 )
		return false;

	socket_scWait sWait;
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


int enviarHeaderCodeaKernel(unsigned char code)
{
	socket_header header;
	header.code = code;
	header.size = sizeof(socket_header);

	return send(conexionKernel, &header, sizeof(socket_header), 0);
}

int enviarPCB()
{
	socket_pcb spcb;
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

