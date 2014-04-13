#include "sockets.h"

#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int conectar(char *ip, int port) {
	//PF_INET: IPv4
	//SOCK_STREAM: Los datos no se pierden ni se duplican
	//IPPROTO_TCP: TCP
	int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock < 0)
		return -1; //No se pudo crear el descriptor a un archivo

	struct sockaddr_in sockaddress;

	memset(&sockaddress, 0, sizeof(struct sockaddr_in));
	sockaddress.sin_family = AF_INET;
	sockaddress.sin_addr.s_addr = inet_addr(ip);
	sockaddress.sin_port = htons(port);

	if( connect(sock, (struct sockaddr*)&sockaddress, sizeof(struct sockaddr)))
		return -1;  //No se pudo conectar

	return sock;
}
