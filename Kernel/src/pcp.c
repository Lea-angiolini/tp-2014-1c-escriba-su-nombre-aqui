#include "pcp.h"
#include <stdio.h>
#include <commons/log.h>
#include "plp.h"

t_log *logpcp;

void *IniciarPcp(void *arg)
{
	logpcp = log_create("log_pcp.txt", "KernelPCP", 1, LOG_LEVEL_TRACE);
	log_info(logpcp, "Thread iniciado");
	multiprogramacion = 2;
	log_info(logpcp, "Thread concluido");
	log_destroy(logpcp);
	return NULL;
}
