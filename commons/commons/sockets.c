#include "log.h"
#include "sockets.h"


#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



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

	if (sock < 0){
		log_error(log, "Error al crear el socket");
		return -1; //No se pudo crear el descriptor a un archivo
	}

	struct sockaddr_in sockaddress;

	memset(&sockaddress, 0, sizeof(struct sockaddr_in));
	sockaddress.sin_family = AF_INET;
	sockaddress.sin_addr.s_addr = inet_addr(ip);
	sockaddress.sin_port = htons(port);

	if( connect(sock, (struct sockaddr*)&sockaddress, sizeof(struct sockaddr))){
		log_error(log, "Error al conectar el socket");
		return -1;  //No se pudo conectar
	}


	return sock;
}


/*
 *	Funcion que crear un nuevo servidor y queda a la escucha de nuevas conexiones.
 *	Al haber un nuevo cliente, crea un nuevo thread llamando a la fn_nuevo_cliente
 *	con parametro el nuevo socket del nuevo cliente.
 *
 */
int crearServidor(int puerto, void* (*fn_nuevo_cliente)( void * socket ), t_log * log ){


	int socketEscucha, socketNuevaConexion;

	struct sockaddr_in socketInfo;
	int optval = 1;

	// Crear un socket:
	// AF_INET: Socket de internet IPv4
	// SOCK_STREAM: Orientado a la conexion, TCP
	// 0: Usar protocolo por defecto para AF_INET-SOCK_STREAM: Protocolo TCP/IPv4
	if ((socketEscucha = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_error( log, ("Error al crear el socket"));
		return -1;
	}

	// Hacer que el SO libere el puerto inmediatamente luego de cerrar el socket.
	setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = INADDR_ANY; //Notar que aca no se usa inet_addr()
	socketInfo.sin_port = htons(puerto);

	// Vincular el socket con una direccion de red almacenada en 'socketInfo'.
	if (bind(socketEscucha, (struct sockaddr*) &socketInfo, sizeof(socketInfo)) != 0) {
		log_error( log, ("Error al bindear socket escucha"));
		return -1;
	}

	// Escuchar nuevas conexiones entrantes.
	if (listen(socketEscucha, 10) != 0) {
		log_error( log, ("Error al poner a escuchar socket"));
		return -1;

	}

	log_info(log, "Escuchando conexiones entrantes...");


	while(1){

		// Aceptar una nueva conexion entrante. Se genera un nuevo socket con la nueva conexion.
		if ((socketNuevaConexion = accept(socketEscucha, NULL, 0)) < 0) {
			log_error( log, ("Error al aceptar conexion entrante"));
			return -1;
		}

		pthread_t thread;
		int * soc = malloc(sizeof(int));
		*soc = socketNuevaConexion;
		pthread_create(&thread, NULL, fn_nuevo_cliente, (void*) soc);
		log_info(log, "Nuevo thread creado");
	}

	return 0;

}
