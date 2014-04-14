/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */




#include "commons/sockets.h"
#include "commons/log.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>


void *ejemplo(void * socket){

	int soc = *((int*) socket);
	free(socket);
	send(soc, "Hola que tal?\n", strlen("Hola que tal?\n"), 0);
	close(soc);
	return 0x0000;

}



int main(void) {

	t_log *log =	log_create("log.txt", "el programon", 1, LOG_LEVEL_TRACE);
	crearServidor(5000, ejemplo, log);
	return 0;

}






