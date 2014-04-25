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
	crearSegmento( 2000 );
	crearSegmento( 5 );
	crearSegmento( 900000 );

	printTodosSegmentos();


}
