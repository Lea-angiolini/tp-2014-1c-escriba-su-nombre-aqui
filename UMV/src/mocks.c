#include <stdint.h>

#include "memoria.h"
#include "Consola.h"
#include "Programa.h"

#include "commons/collections/list.h"
#include "commons/log.h"
#include "config.h"

extern t_log * logger;


void ejecutar(){


	log_info( logger, "Creando un programa para que la cpu pueda acceder. ");
	crearPrograma( 1, malloc(1), malloc(1), malloc(1), 1, 1, 1, 1024 );


	/*
	crearSegmento( 1000 );
	Segmento * seg = crearSegmento( 2000 );
	crearSegmento( 5 );
	borrarSegmento( seg );
	Segmento * seg2 = crearSegmento( 900 );
	borrarSegmento( seg2 );
	crearSegmento( 75 );
	crearSegmento( 2000 );
	crearSegmento( 1 );
	Segmento * seg3 = crearSegmento( 30 );
	borrarSegmento( seg3 );
	crearSegmento( 1 );
	crearSegmento( 10 );



	printf("MOstrando la tabla terminada \n\n");
	printTodosSegmentos();
	printf("Mostrando la tabla compactada \n\n");
	compactar();
	printTodosSegmentos();
	*/

}
