#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commons/log.h"
#include "commons/string.h"
#include "commons/collections/list.h"

#include "Segmento.h"
#include "Programa.h"
#include <pthread.h>

void * iniciarConsola(void * params);
uint32_t parsearComando(char * comando);
void operacionesConSegmentos();
void modificarAlgoCreacionSegmentos();
void modificarRetardoUMV();
void generarDump();
void requisitosOperacionSegmento( char operacion);
void solicitarPosicion( uint32_t base, uint32_t offset, uint32_t tamanio);
void escribirPosicion();
void imprimirMemoria();
void printSegmentos(t_list * segmentos);
void printSegmentosHeaders();
void printSegmentosPorPrograma();
void printTodosSegmentos();
void printSegmento(Segmento * segmento);
void buscarProgramaEImprimirSegmentos();
void printEspacioLibre(uint32_t inicioEspacio, uint32_t finEspacio);
bool verificarRequisitos( uint32_t programa, uint32_t base);

void imprimirSegmentosDe(Programa * programaAImprimir);

#endif
