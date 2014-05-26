#include "Segmento.h"
#include "Programa.h"
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
void printSegmentosPorPrograma();
void printTodosSegmentos();
void printSegmento( Segmento * segmento );
void buscarProgramaEImprimirSegmentos();

void imprimirSegmentosDe(Programa * programaAImprimir);
