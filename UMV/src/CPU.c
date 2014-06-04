#include "commons/collections/list.h"
#include "commons/log.h"
#include "commons/sockets.h"
#include "CPU.h"
#include "Programa.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFF_SIZE 1024

extern t_log * logger;

//TODO liberar esta lista al finalizar el programa
extern t_list * cpus;

int contadorCpuId;




 int procesarSolicitudDeLinea( CPU * cpu, socket_obtenerLineaCodigo * solicitud ) {
	//log_info( logger, "La CPU ID: %d solicito la linea de programa: %d del pid XXX", cpu->cpuId, solicitud->numero_linea_Codigo );
	log_info( logger, "Alguna cpu solicito una linea de codigo" );

	socket_responderLineaCodigo * paquete = malloc( sizeof( socket_responderLineaCodigo ) ) ;
	//respuesta->lineaCodigo = "a = 3";
	paquete->numero_linea_Codigo = 3;

	return enviarPaquete( cpu->socket, (void *) paquete, sizeof( socket_responderLineaCodigo ) , 'a', logger );

}





//En estos dos casos tiene que hacer los siguientes pasos
// 1. Buscar el programa que esta procesando
// 2. Obtener el segmento correspondiente al offset solicitado
// 3. Teniendo el socket delegarle la operacion
// 4. Responder dependiendo del estado

int procesarSolicitudLecturaMemoria( CPU * cpu, socket_leerMemoria * solicitud ) {

	/*if( cpu->pidProcesando != solicitud->pdi) {
		log_error(logger, "La cpu solicito un pedido de memoria de un pid que no esta procesando UMV/src/CPU.c -> procesarSolicitudLecturaMemoria ");
		return -1;
	}*/

	Programa * programa;
	programa = buscarPrograma( solicitud->pdi );
	Segmento * segmento;
	segmento = buscarSegmentoEnPrograma(programa, solicitud->base);

	if( segmento == NULL ) {
		log_error(logger, "No se encuentra el segmento especificado | UMV/src/cpu.c -> procesarSolicitudLecturaMemoria");
		return -1;
	}

	uint32_t tamanioParaOperar;
	tamanioParaOperar = segmento->finReal - (segmento->inicioReal + solicitud->offset) + 1;
	tamanioParaOperar = 2000000000;

	socket_RespuestaLeerMemoria * respuesta = malloc(sizeof (socket_RespuestaLeerMemoria));
	memset( respuesta, 0, sizeof (socket_RespuestaLeerMemoria) );
	respuesta->header.size = sizeof(socket_RespuestaLeerMemoria);

	if(	solicitud->length > tamanioParaOperar ) {
		log_error(logger, "Segmentation fault, length: %d, tamanioParaOperar: %d, base: %d, offset: %d | UMV/src/cpu.c -> procesarSolicitudLecturaMemoria", solicitud->length, tamanioParaOperar, solicitud->base, solicitud->offset );
		respuesta->status = false;
	}else{
		respuesta->status = true;
		memLeer( segmento, respuesta->data, solicitud->offset, solicitud->length);
		log_info( logger, "Se leyo la data: %s", respuesta->data );
	}

	uint32_t enviado;
	enviado = send(cpu->socket, respuesta, sizeof(socket_RespuestaLeerMemoria), 0);

	if( enviado == -1 ) {
		log_error(logger, "La respuesta a lectura de memoria no se ha realizado con exito | UMV/src/cpu.c -> procesarSolicitudLecturaMemoria");
		return -1;
	}

	log_info(logger, "La respuesta a lectura de memoria se ha realizado con exito");

	return 1;

}




int procesarSolicitudEscrituraMemoria( CPU * cpu, socket_guardarEnMemoria * solicitud ) {


	/*if( cpu->pidProcesando != solicitud->pdi){
		log_error(logger, "La cpu solicito un pedido de memoria de un pid que no esta procesando  UMV/src/CPU.c -> procesarSolicitudEscrituraMemoria ");
		return -1;
	}*/

	Programa * programa;
	programa = buscarPrograma( solicitud->pdi);
	Segmento * segmento;
	segmento = buscarSegmentoEnPrograma(programa, solicitud->base);

	if( segmento == NULL ) {
		log_error(logger, "No se encuentra el segmento especificado | UMV/src/cpu.c -> procesarSolicitudEscrituraMemoria");
		return -1;
	}

	uint32_t tamanioParaOperar;
	tamanioParaOperar = segmento->finReal - (segmento->inicioReal + solicitud->offset) + 1;

	socket_RespuestaGuardarEnMemoria * respuesta = malloc(sizeof (socket_RespuestaGuardarEnMemoria));
	respuesta->header.size = sizeof(socket_RespuestaGuardarEnMemoria);

	if(	solicitud->length > tamanioParaOperar ) {
		log_error(logger, "Segmentation fault, length: %d, tamanioParaOperar: %d, base: %d, offset: %d | UMV/src/cpu.c -> procesarSolicitudEscrituraMemoria", solicitud->length, tamanioParaOperar, solicitud->base, solicitud->offset );
		respuesta->status = false;
	}else{
		respuesta->status = true;
		memCopi( segmento, solicitud->offset, solicitud->data, solicitud->length );
		log_info( logger, "Se guardo la data: %s", solicitud->data );
	}

	uint32_t enviado;
	enviado = send( cpu->socket, respuesta, sizeof( socket_RespuestaGuardarEnMemoria ), 0 );

	if( enviado == -1 ) {
		log_error(logger, "La respuesta a escribir en memoria no se ha realizado con exito | UMV/src/cpu.c -> procesarSolicitudEscrituraMemoria");
		return -1;
	}

	log_info(logger, "La respuesta a escribir en memoria se ha realizado con exito");

	return 1;
}




int recibirYProcesarMensajesCpu( CPU * cpu ) {

	int todoSaleBien = 1;

	while( todoSaleBien > 0 ){

		log_info( logger, "Esperando un mensaje de la CPU..." );
		void * paquete = recibirPaquete( cpu->socket , 0, 'a', logger );

		if( paquete == NULL ){
			log_error( logger, "Se ha desconectado la CPU%d por causas desconocidas", cpu->cpuId);
			log_info( logger, "Destruyendo programa activo de la CPU%d", cpu->cpuId);
			uint32_t programaDestruido;
			programaDestruido = destruirPrograma( cpu->pidProcesando);
			if( programaDestruido == true){
				log_info( logger, "El programa activo de la CPU%d se ha destruido correctamente", cpu->cpuId);
			}else
				log_error( logger, "El programa activo de la CPU%d no se ha destruido correctamente", cpu->cpuId);

			return -1;
		}

		socket_header * header = ( socket_header * ) paquete;

		switch ( header->code ){
			case 'a':
				todoSaleBien = procesarSolicitudDeLinea( cpu, (socket_obtenerLineaCodigo *) paquete );
				break;
			case 'b':
				todoSaleBien = procesarSolicitudLecturaMemoria( cpu, (socket_leerMemoria *) paquete );
								break;

			case 'c':
				todoSaleBien = procesarSolicitudEscrituraMemoria( cpu, (socket_guardarEnMemoria *) paquete );
								break;
			default:
				//TODO decidir que hacer aca
				log_error( logger, "La CPU ID: %d envio un mensaje que no se entiende... ???", cpu->cpuId );
				break;

		}

		free( paquete );

	}

	return todoSaleBien;

}





void  fnNuevoCpu( int * socketPtr ){

	log_info( logger, "Se conecto un nuevo CPU" );


	//TODO al desconectar sacar el cpu de la lista de CPUs
	contadorCpuId++;
	CPU * cpu = malloc(sizeof(CPU));
	cpu->socket = * socketPtr;
	cpu->cpuId = contadorCpuId;
	free( socketPtr );
	list_add( cpus, cpu );


	if( recibirYProcesarMensajesCpu( cpu ) > 0 ){
		log_info( logger, "Se desconecto un cpu correctamente" );
	}else{
		log_error( logger, "Hubo un problema con el cpu ID %d: " , cpu->cpuId );
	}

	//TODO !!!! IMPORTANTE que paso acaaa ??
	close( cpu->socket );



}













