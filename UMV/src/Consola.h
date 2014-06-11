#include "Segmento.h"
#include "Programa.h"
#include <pthread.h>
#include "commons/log.h"
#include "commons/collections/list.h"

void * iniciarConsola(void * params);
uint32_t parsearComando(char * comando);
void operacionesConSegmentos();
void modificarAlgoCreacionSegmentos();
void modificarRetardoUMV();
void generarDump();
void requisitosOperacionSegmento( char operacion);
void solicitarPosicion(uint32_t programa, uint32_t base, uint32_t offset, uint32_t tamanio);
void escribirPosicion();
void imprimirMemoria();
void printSegmentos(t_list * segmentos);
void printSegmentosHeaders();
void printSegmentosPorPrograma();
void printTodosSegmentos();
void printSegmento(Segmento * segmento);
void buscarProgramaEImprimirSegmentos();
void printEspacioLibre(uint32_t inicioEspacio, uint32_t finEspacio);

void imprimirSegmentosDe(Programa * programaAImprimir);
