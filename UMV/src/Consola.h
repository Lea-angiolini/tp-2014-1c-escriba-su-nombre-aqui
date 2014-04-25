#include "Segmento.h"

#include "commons/log.h"
#include "commons/collections/list.h"



void * iniciarConsola( void * params );
int parsearComando( char * comando ) ;


void printSegmentos( t_list * segmentos );
void printSegmentosHeaders();
void printTodosSegmentos();
void printSegmento( Segmento * segmento );
