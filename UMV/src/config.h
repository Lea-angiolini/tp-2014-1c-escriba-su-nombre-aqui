#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "commons/sockets.h"
#include "commons/log.h"
#include "commons/config.h"
#include "commons/collections/dictionary.h"
#include "commons/collections/list.h"
#include "commons/collections/queue.h"

#include "Consola.h"
#include "Kernel.h"
#include "CPU.h"
#include "Programa.h"
#include "mocks.h"


bool cargar_config(char *config);
void * handShake( void * socket );
void * crearConexiones();
bool leerConfiguraciones( char * config);
void destruir_config();
