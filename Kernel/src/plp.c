#include "plp.h"
#include "colas.h"
#include "config.h"

#define CALCULAR_PRIORIDAD(e,f,t) (5 * e + 3 * f + t)

t_log *logplp;

uint32_t nextProcessId = 1;
uint32_t multiprogramacion = 0;
pthread_mutex_t multiprogramacionMutex = PTHREAD_MUTEX_INITIALIZER;

extern sem_t semKernel;
extern sem_t dispatcherReady;

int socketUMV;


void *IniciarPlp(void *arg) {
	logplp = log_create("log_plp.txt", "KernelPLP", 1, LOG_LEVEL_TRACE);
	log_debug(logplp, "Thread iniciado");

	if( conectarUMV() )
		iniciarServidorProgramas();

	log_debug(logplp, "Thread concluido");
	log_destroy(logplp);

	sem_post(&semKernel);
	return NULL;
}

bool iniciarServidorProgramas()
{
	bool nuevoMensaje(int socketPrograma) {
		if (recibirYprocesarScript(socketPrograma) == false) {
			desconexionCliente();
			return false;
		}

		return true;
	}

	log_debug(logplp, "Iniciando servidor de Programas");

	if (crearServidorNoBloqueante(config_get_int_value(config, "PUERTO_PROG"), nuevoMensaje, logplp) == -1) {
		log_error(logplp, "Hubo un problema en el servidor receptor de Programas");
		return false;
	}

	return true;
}

bool conectarUMV()
{
	socketUMV = conectar(config_get_string_value(config, "IP_UMV"), config_get_int_value(config, "PUERTO_UMV"), logplp);

	if (socketUMV == -1) {
		log_error(logplp, "No se pudo establecer la conexion con la UMV");
		return false;
	}

	log_info(logplp, "Conectado con la UMV");

	socket_header handshake;

	handshake.size = sizeof(socket_header);
	handshake.code = 'k'; //Kernel

	if( send(socketUMV, &handshake, sizeof(socket_header), 0) <= 0 )
		return false;

	return true;
}

void MoverNewAReady()
{
	log_info(logplp, "Ordenando la cola NEW segun algoritmo de planificacion SJN");

	bool sjnAlgorithm(pcb_t *a, pcb_t *b)
	{
		return a->prioridad < b->prioridad;
	}
	list_sort(newQueue->elements, sjnAlgorithm);

	log_info(logplp, "Moviendo PCB de la cola NEW a READY");

	pthread_mutex_lock(&readyQueueMutex);
	queue_push(readyQueue, queue_pop(newQueue));
	pthread_mutex_unlock(&readyQueueMutex);

	pthread_mutex_lock(&multiprogramacionMutex);
	multiprogramacion++;
	pthread_mutex_unlock(&multiprogramacionMutex);

	//Llamada a dispatcher del PCP para avisar que hay un nuevo trabajo pendiente
	sem_post(&dispatcherReady);
}

void puedoMoverNewAReady()
{
	log_info(logplp, "Verificando grado de multiprogramacion");

	if(multiprogramacion < config_get_int_value(config, "MULTIPROGRAMACION") && !queue_is_empty(newQueue)) {
		MoverNewAReady();
	}
}

void desconexionCliente()
{
	log_info(logplp, "Se ha desconectado un Programa");
	puedoMoverNewAReady();
}

bool recibirYprocesarScript(int socketPrograma) {
	//Pidiendo script ansisop
	socket_header header;

	if( recv(socketPrograma, &header, sizeof(socket_header), MSG_WAITALL) != sizeof(socket_header) )
		return false;

	int scriptSize = header.size - sizeof(header);

	char *script = malloc(scriptSize + 1);
	memset(script, 0x00, scriptSize + 1);

	log_info(logplp, "Esperando a recibir un script ansisop");

	if( recv(socketPrograma, script, scriptSize, MSG_WAITALL) != scriptSize )
		return false;

	log_info(logplp, "Script ansisop recibido");

	//ansisop preprocesador
	t_metadata_program *scriptMetadata = metadata_desde_literal(script);

	log_info(logplp, "Pidiendole memoria a la UMV para que pueda correr el script ansisop");

	socket_pedirMemoria pedirMemoria;
	pedirMemoria.header.size = sizeof(pedirMemoria);

	pedirMemoria.codeSegmentSize = scriptSize + 1;
	pedirMemoria.stackSegmentSize = config_get_int_value(config, "STACK_SIZE");
	pedirMemoria.etiquetasSegmentSize = scriptMetadata->etiquetas_size;
	pedirMemoria.instruccionesSegmentSize = scriptMetadata->instrucciones_size * sizeof(t_intructions);

	if( send(socketUMV, &pedirMemoria, sizeof(socket_pedirMemoria), 0) < 0 ){
		log_error(logplp,"No se puedo pedir memoria a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	socket_respuesta respuesta;

	if( recv(socketUMV, &respuesta, sizeof(socket_respuesta), MSG_WAITALL) != sizeof(socket_respuesta) ){
		log_error(logplp,"No se recibio respuesta de la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	if(respuesta.valor == true)
	{
		log_info(logplp, "La UMV informo que pudo alojar la memoria necesaria para el script ansisop");
		log_info(logplp, "Enviando a la UMV los datos a guardar en los segmentos");

		if( send(socketUMV, &nextProcessId, sizeof(nextProcessId), 0) < 0 ){
			log_error(logplp,"No se pudo enviar nextProcessId a la UMV. Desconectando");
			sem_post(&semKernel);
			return false;
		}

		if( send(socketUMV, script, pedirMemoria.codeSegmentSize, 0) < 0 ){
			log_error(logplp,"No se pudo enviar script a la UMV. Desconectando");
			sem_post(&semKernel);
			return false;
		}

		if( send(socketUMV, scriptMetadata->etiquetas, pedirMemoria.etiquetasSegmentSize, 0) < 0 ){
			log_error(logplp,"No se pudo enviar etiquetas a la UMV. Desconectando");
			sem_post(&semKernel);
			return false;
		}

		if( send(socketUMV, scriptMetadata->instrucciones_serializado, pedirMemoria.instruccionesSegmentSize, 0) < 0 ){
			log_error(logplp,"No se puedo enviar instrucciones serializado a la UMV. Desconectando");
			sem_post(&semKernel);
			return false;
		}

		socket_umvpcb umvpcb;

		if( recv(socketUMV, &umvpcb, sizeof(socket_umvpcb), MSG_WAITALL) != sizeof(socket_umvpcb) ){
			log_error(logplp,"No se recibio pcb de la UMV. Desconectando");
			sem_post(&semKernel);
			return false;
		}

		pcb_t *pcb = malloc(sizeof(pcb_t));

		pcb->id = nextProcessId; nextProcessId++;

		pcb->codeSegment = umvpcb.codeSegment;
		pcb->stackSegment = umvpcb.stackSegment;
		pcb->stackCursor = umvpcb.stackSegment;
		pcb->codeIndex = umvpcb.codeIndex;
		pcb->etiquetaIndex = umvpcb.etiquetaIndex;
		pcb->etiquetasSize = scriptMetadata->etiquetas_size;
		pcb->programCounter = scriptMetadata->instruccion_inicio;
		pcb->contextSize = 0;

		pcb->programaSocket = socketPrograma;
		pcb->prioridad = CALCULAR_PRIORIDAD(scriptMetadata->cantidad_de_etiquetas, scriptMetadata->cantidad_de_funciones, scriptMetadata->instrucciones_size);
		pcb->lastErrorCode = 0;

		queue_push(newQueue, pcb);
		log_info(logplp, "Segmentos cargados en la UMV y PCB generada en la cola NEW");

		puedoMoverNewAReady();
	} else {
		log_error(logplp, "La UMV informo que no pudo alojar la memoria necesaria para el script ansisop");
		log_info(logplp, "Informandole a Programa que el script no se puede procesar por el momento");

		mensajeYDesconexionPrograma(socketPrograma, "No hay memoria suficiente en este momento para ejecutar este script. Intentelo mas tarde");

		return false;
	}

	metadata_destruir(scriptMetadata);
	free(script);

	return true;
}

