#include "Segmento.h"
#include <pthread.h>
#include "commons/log.h"
#include "commons/collections/list.h"



void * iniciarConsola( void * params );
int parsearComando( char * comando ) ;
void operacionesConSegmentos();
void modificarAlgoCreacionSegmentos();
void modificarRetardoUMV();
void generarDump();

void printSegmentos( t_list * segmentos );
void printSegmentosHeaders();
void printTodosSegmentos();
void printSegmento( Segmento * segmento );
