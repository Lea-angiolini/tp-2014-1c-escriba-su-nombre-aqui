#include <stdint.h>

#include "memoria.h"
#include "Consola.h"

#include "commons/collections/list.h"
#include "commons/log.h"


extern t_log * logger;
extern t_list * cpus; //Lista en la que se van a guardar toda la info de cada cpu que se conecte
extern t_list * programas;
extern t_list * tabla_segmentos;

extern uint32_t memoria_size;

void ejecutar(){

	//log_info( logger, "Creando un segmento de %d bytes", memoria_size);
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



}
