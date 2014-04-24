#include "log.h"
#include "sockets.h"

#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>

/*
 * Funcion que crea un socket cliente para conectarlo a la ip y puerto pasados como parametros
 * Devuelve -1 si hubo error o el valor del socket
 *
 */
int conectar(char *ip, int port, t_log * log) {
	//PF_INET: IPv4
	//SOCK_STREAM: Los datos no se pierden ni se duplican
	//IPPROTO_TCP: TCP
	log_info(log, "Conectando");
	int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock < 0) {
		log_error(log, "Error al crear el socket");
		return -1; //No se pudo crear el descriptor a un archivo
	}

	struct sockaddr_in sockaddress;

	memset(&sockaddress, 0, sizeof(struct sockaddr_in));
	sockaddress.sin_family = AF_INET;
	sockaddress.sin_addr.s_addr = inet_addr(ip);
	sockaddress.sin_port = htons(port);

	if (connect(sock, (struct sockaddr*) &sockaddress,
			sizeof(struct sockaddr))) {
		log_error(log, "Error al conectar el socket");
		return -1;  //No se pudo conectar
	}

	return sock;
}

int crearYbindearSocket(int puerto, t_log * log)
{
	int socketEscucha;
	struct sockaddr_in socketInfo;
	int optval = 1;

	// Crear un socket:
	// AF_INET: Socket de internet IPv4
	// SOCK_STREAM: Orientado a la conexion, TCP
	// 0: Usar protocolo por defecto para AF_INET-SOCK_STREAM: Protocolo TCP/IPv4
	if ((socketEscucha = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		log_error(log, ("Error al crear el socket"));
		return -1;
	}

	// Hacer que el SO libere el puerto inmediatamente luego de cerrar el socket.
	setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	memset(&socketInfo, 0, sizeof(socketInfo));
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = INADDR_ANY; //Notar que aca no se usa inet_addr()
	socketInfo.sin_port = htons(puerto);

	// Vincular el socket con una direccion de red almacenada en 'socketInfo'.
	if (bind(socketEscucha, (struct sockaddr*) &socketInfo, sizeof(socketInfo))
			!= 0) {
		log_error(log, "Error al bindear socket escucha");
		return -1;
	}

	return socketEscucha;
}

/*
 *	Funcion que crear un nuevo servidor y queda a la escucha de nuevas conexiones.
 *	Al haber un nuevo cliente, crea un nuevo thread llamando a la fn_nuevo_cliente
 *	con parametro el nuevo socket del nuevo cliente.
 *
 */
int crearServidor(int puerto, void* (*fn_nuevo_cliente)(void * socket), t_log * log) {

	int socketEscucha, socketNuevaConexion;

	socketEscucha = crearYbindearSocket(puerto, log);

	if(socketEscucha == -1)
		return -1;

	// Escuchar nuevas conexiones entrantes.
	if (listen(socketEscucha, SOMAXCONN) != 0) {
		log_error(log, "Error al poner a escuchar socket");
		return -1;

	}

	log_info(log, "Escuchando conexiones entrantes...");

	while (1) {

		// Aceptar una nueva conexion entrante. Se genera un nuevo socket con la nueva conexion.
		log_info(log, "Esperando nueva conexion...");
		if ((socketNuevaConexion = accept(socketEscucha, NULL, 0)) < 0) {
			log_error(log, "Error al aceptar conexion entrante !! ");
			return -1;
		}

		pthread_t thread;
		int * soc = malloc(sizeof(int));
		*soc = socketNuevaConexion;
		log_info(log, "Se establecio la nueva conexion, creando el thread...");
		pthread_create(&thread, NULL, fn_nuevo_cliente, (void*) soc);
		log_info(log, "Nuevo thread creado");

	}

	return 0;

}




int crearServidorNoBloqueante(int puerto, bool (*fn_nuevo_mensaje)(void *socket), t_log * log) {
	int i, rc, optval = 1;
	int socketEscucha, max_sd, new_sd;
	bool desc_ready, end_server = false;
	int close_conn;
	fd_set master_set, working_set;

	socketEscucha = crearYbindearSocket(puerto, log);

	if(socketEscucha == -1)
		return -1;

	// Hacer que el socket se comporte como no bloqueante
	ioctl(socketEscucha, FIONBIO, &optval);

	// Escuchar nuevas conexiones entrantes.
	if (listen(socketEscucha, SOMAXCONN) != 0) {
		log_error(log, "Error al poner a escuchar socket");
		return -1;
	}

	///////////////////////////////////
	FD_ZERO(&master_set);
	max_sd = socketEscucha;
	FD_SET(socketEscucha, &master_set);

	do {
		memcpy(&working_set, &master_set, sizeof(master_set));

		log_info(log, "Esperando en el select algun evento...");
		rc = select(max_sd + 1, &working_set, NULL, NULL, NULL );

		if (rc <= 0) {
			log_error(log, "Select fallo o termino su tiempo de espera");
			break;
		}

		desc_ready = rc;
		for (i = 0; i <= max_sd && desc_ready > 0; ++i) {
			//Viendo si el descriptor se encuentra listo
			if (FD_ISSET(i, &working_set)) {
				//Se encontraron descriptores listos para leer
				desc_ready -= 1;

				//Ver si se trata de una conexion pendiente
				if (i == socketEscucha) {
					//Aceptando conexiones pendientes
					do {
						new_sd = accept(socketEscucha, NULL, NULL );
						if (new_sd < 0) {
							if (errno != EWOULDBLOCK) {
								log_error(log, "Error al aceptar conexion entrante");
								end_server = true;
							}
							break;
						}

						FD_SET(new_sd, &master_set);
						if (new_sd > max_sd)
							max_sd = new_sd;

						//Nuevo cliente conectado
					} while (new_sd != -1);
				}

				//No se trata de una conexion pendiente, si no de una que ya existia previamente
				else {
					//Obtener paquetes recibidos
					close_conn = !fn_nuevo_mensaje(i);

					//Si es necesario, borrar conexion actual
					if (close_conn) {
						close(i);
						FD_CLR(i, &master_set);
						if (i == max_sd) {
							while (FD_ISSET(max_sd, &master_set) == false)
								max_sd -= 1;
						}
					}
				}
			}
		}

	} while (end_server == false);

	//Cerrar todas las conexiones que quedaron abiertas
	for (i = 0; i <= max_sd; ++i) {
		if (FD_ISSET(i, &master_set))
			close(i);
	}

	return -1;
}









void * enviarYRecibirPaquete( int socket, void * mensaje, uint32_t sizeSend, uint32_t sizeReceive, char sendCode, char receiveCode, t_log * logger ) {

	if( enviarPaquete( socket,  mensaje,  sizeSend, sendCode, logger ) < 0 ){
		return NULL;
	}
	return recibirPaquete( socket, sizeReceive, receiveCode, logger );
}

int enviarPaquete( int socket, void * mensaje, uint32_t sizeSend, char sendCode, t_log * logger ) {

	socket_header header;
	header.size = sizeSend;
	header.code = sendCode;

	memcpy( mensaje, &header, sizeof( socket_header ) );

	log_debug( logger, "Enviando paquete de tamaño %d.", sizeSend );
	if ( send( socket, mensaje, sizeSend, 0 ) < 0 ){
		return -1;
	}

	return 1;
}

//TODO Revisar tamanio t_size == uint32_t ????
void * recibirPaquete( int socket, t_size sizeReceive, char receiveCode, t_log * logger ) {

	socket_header headerRespuesta;
	int tamRecibido = recv( socket, &headerRespuesta, sizeof(socket_header), 0);
	if ( tamRecibido < 0 ){
		//log_error( logger, "Se recibio mal un paquete");
		return NULL;
	}else if (tamRecibido == 0)  {
		return NULL;
	}

	//log_info( logger, "Se recibio un paquete" );
	/*
	if( sizeReceive != NULL && (headeRespuesta.size) != sizeReceive ){
		return NULL;
	}

	if( receiveCode != NULL && (headeRespuesta.code) != receiveCode ){
		return NULL;
	}
	*/

	//log_info( logger, "Reservando el espacio de memoria..." );
	void * paqueteRespuesta = malloc( headerRespuesta.size );
	//Aritmetica de punteros !!!!!!
	//log_info( logger, "Leyendo el paquete de tamaño: %d", headerRespuesta.size );
	recv( socket, ( paqueteRespuesta + sizeof( socket_header ) ), headerRespuesta.size, 0);
	//recv( socket, ( &paqueteRespuesta + sizeof( socket_header ) ), headerRespuesta.size, 0);
	//log_info( logger, "Se leyo el paquete y se va a agregarle el header" );
	memcpy( paqueteRespuesta, &headerRespuesta, sizeof( socket_header ) );

	return paqueteRespuesta;

}







