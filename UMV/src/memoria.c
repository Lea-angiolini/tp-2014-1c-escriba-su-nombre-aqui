#include "memoria.h"
#include "config.h"

extern t_log * logger;
extern FILE *mensajesUMV;
extern pthread_rwlock_t lockEscrituraLectura;

uint32_t contadorId = 0;


Segmento * crearYllenarSegmento(uint32_t tamanio, void * segmento) { //TODO Habria que agregarle un id de tipo al segmento
	pthread_rwlock_rdlock(&lockEscrituraLectura);
	Segmento * segmentoAllenar = crearSegmento(tamanio);
	memCopi(segmentoAllenar, 0, segmento, tamanio);
	pthread_rwlock_unlock(&lockEscrituraLectura);
	return segmentoAllenar;
}

Segmento * crearSegmento(uint32_t tamanio) {
	//aca no lockeo por escritura ni lectura porque, como tanto la consola como los CPU usan esta funcion, puede haber
	//un quilombo si se hace un rdlock adentro de un wrlock. Lockeo tanto en la consola como en la CPU, en vez de aca.
	//log_info(logger, "El tamanio del segmento es %d", tamanio);
	fprintf( mensajesUMV, "El tamanio del segmento es %d\n", tamanio);

	Segmento * elNuevo = NULL;

	if (list_size(tabla_segmentos) == 0 && memoria_size > tamanio) {
		elNuevo = new_Segmento(0, tamanio);
		} else {
			if(modoActualCreacionSegmentos == WORSTFIT){
			elNuevo = crearSegmentoWorstFit(tamanio);
				} else {
					elNuevo = crearSegmentoFirstFit(tamanio);
					}
			}

	if (elNuevo != NULL ) {
		elNuevo->id = contadorId;
		contadorId++;

		list_add(tabla_segmentos, elNuevo);
	}
	log_info( logger, "Segmento creado, ahora hay %d", list_size(tabla_segmentos));
	fprintf( mensajesUMV, "Segmento creado, ahora hay %d\n", list_size(tabla_segmentos));

	return elNuevo;

}

Segmento * crearSegmentoFirstFit( uint32_t tamanio) {

	pthread_rwlock_rdlock(&lockEscrituraLectura);
	t_list * huequitos = crearListaEspacioDisponible();

	uint32_t i = 0;
	Segmento * huequito;
	for (i = 0; i < list_size(huequitos); i++) {
		huequito = (Segmento*) list_get(huequitos, i);
		if ((huequito->finReal - huequito->inicioReal + 1) >= tamanio) {

			Segmento * elNuevo = new_Segmento(huequito->inicioReal,
					huequito->inicioReal + tamanio);
			pthread_rwlock_unlock(&lockEscrituraLectura);
			list_destroy(huequitos);
			return elNuevo;
		}
	}

	pthread_rwlock_unlock(&lockEscrituraLectura);
	compactar();

	list_destroy(huequitos);
	return crearSegmentoFirstFit( tamanio);
}

Segmento * crearSegmentoWorstFit( uint32_t tamanio) {

	t_list * huequitos = crearListaEspacioDisponible();
	uint32_t i = 0, tamanioMax = 0, tamanioHuequito = 0;
	pthread_rwlock_rdlock(&lockEscrituraLectura);
	Segmento * nuevoSegmento = NULL;

	for (i = 0; i < list_size(huequitos); i++) {
		Segmento * huequito = (Segmento*) list_get(huequitos, i);
		tamanioHuequito = huequito->finReal - huequito->inicioReal + 1;
		if (tamanioHuequito >= tamanioMax && tamanioHuequito >= tamanio) {
			nuevoSegmento = huequito;
			tamanioMax = tamanioHuequito;
		}
	}
	if (tamanioMax >= tamanio) {
		pthread_rwlock_unlock(&lockEscrituraLectura);
		list_destroy(huequitos);
		return new_Segmento(nuevoSegmento->inicioReal,
				nuevoSegmento->inicioReal + tamanio);
	}

	pthread_rwlock_unlock(&lockEscrituraLectura);
	compactar();
	list_destroy(huequitos);
	return crearSegmentoWorstFit(  tamanio );
}

bool segmentoEsAnterior(void * seg1, void * seg2) {
	Segmento * segmento1 = (Segmento *) seg1;
	Segmento * segmento2 = (Segmento *) seg2;
	return segmento1->inicioReal < segmento2->inicioReal;
}


void ordenarTablaSegmentos() {
	list_sort(tabla_segmentos, &segmentoEsAnterior);
}

/*
 * Crea una lista de "Segmentos" indicando el inicio y fin de cada espacio libre.
 * Esta lista en si lo que hace es crear todos los segmentos posibles ocupando el mayor espacio posible
 * En fin devuelve los huequitos vacios
 *
 */
//TODO usar semaforo
//TODO validar, son necesarios los +1 ?
t_list * crearListaEspacioDisponible() {
	pthread_rwlock_rdlock(&lockEscrituraLectura);
	ordenarTablaSegmentos();

	t_list * lista = list_create();

	if (list_size(tabla_segmentos) == 0) {

		Segmento * segmento = new_Segmento(0, memoria_size);
		list_add(lista, segmento);

	} else {

		Segmento * primerSegmento = (Segmento *) list_get(tabla_segmentos, 0);

		if (primerSegmento->inicioReal != 0) {
			Segmento * segmentoInicial = new_Segmento(0,
					primerSegmento->inicioReal - 1);
			list_add(lista, segmentoInicial);
		}

		uint32_t i = 0;
		for (i = 0; i < list_size(tabla_segmentos) - 1; i++) {

			Segmento * segmento1 = (Segmento *) list_get(tabla_segmentos, i);
			Segmento * segmento2 = (Segmento *) list_get(tabla_segmentos,
					i + 1);

			if (segmento1->finReal != (segmento2->inicioReal - 1) ) {
				Segmento * segmentoIntermedio = new_Segmento(
						segmento1->finReal + 1, segmento2->inicioReal);
				list_add(lista, segmentoIntermedio);
				}
			}

		Segmento * ultimoSegmento = (Segmento *) list_get(tabla_segmentos,
					list_size(tabla_segmentos) - 1);
			if ( ultimoSegmento->finReal != memoria_size ) {
				Segmento * segmentoFinal;
				segmentoFinal = new_Segmento(ultimoSegmento->finReal + 1, memoria_size);
				list_add(lista, segmentoFinal);
			}
	}

	pthread_rwlock_unlock(&lockEscrituraLectura);
	return lista;

}

void borrarSegmento(Segmento * segmentoABorrar) {
	bool matchearSegmento( Segmento * segmento){
		return segmento->id == segmentoABorrar->id;
	}

	list_remove_and_destroy_by_condition( tabla_segmentos, matchearSegmento, free);
}


Segmento * buscarSegmentoEnTabla( uint32_t base){
		bool matchearSegmento( Segmento * segmento){
			return segmento->inicioReal == base;
		}
		return list_find( tabla_segmentos, matchearSegmento);
}


uint32_t memoriaOcupada() {
	uint32_t i = 0, sumador = 0;
	for (i = 0; i < list_size(tabla_segmentos); i++) {
		Segmento * segmento = (Segmento *) list_get(tabla_segmentos, i);
		sumador += tamanioSegmento( segmento );
	}
	return sumador;
}

uint32_t memoriaLibre() {
	return memoria_size - memoriaOcupada();
}

void compactar() {
	pthread_rwlock_wrlock(&lockEscrituraLectura);
	ordenarTablaSegmentos();
	uint32_t u = 0;
	Segmento * primerSegmento = (Segmento *) list_get(tabla_segmentos, u);

	while( primerSegmento->inicioReal == SEGMENTOVACIO){
		u++;
		primerSegmento = (Segmento *) list_get(tabla_segmentos, u);
		}

	if (primerSegmento->inicioReal != 0) {
		moverSegmento(primerSegmento, 0);
	}
	uint32_t i = 0;
	for (i = 0; i < list_size(tabla_segmentos) - 1; i++) {
		Segmento * segmentoMovido = (Segmento *) list_get(tabla_segmentos, i);
		Segmento * segmentoAmover = (Segmento *) list_get(tabla_segmentos,
				i + 1);
		if( segmentoMovido->finReal != (segmentoAmover->inicioReal -1) ){
			moverSegmento(segmentoAmover, segmentoMovido->finReal + 1);
		}

	}
	log_info( logger, "Se ha compactado correctamente");
	fprintf( mensajesUMV, "Se ha compactado correctamente \n");
	printSegmentos(tabla_segmentos);
	pthread_rwlock_unlock(&lockEscrituraLectura);
	return;
}

void moverSegmento(Segmento * segmento, uint32_t posicion) {

	uint32_t tamanio = tamanioSegmento(segmento);
	uint32_t nuevoInicio = posicion;
	uint32_t nuevoFin = posicion + tamanio - 1;

	//TODO
	memcpy( memoria + nuevoInicio,
			 memoria + segmento->inicioReal, tamanio);

	segmento->inicioReal = nuevoInicio;
	segmento->finReal = nuevoFin;
	return;

}

uint32_t tamanioSegmento(Segmento * segmento) {
	return (segmento->finReal - segmento->inicioReal + 1);
}

uint32_t solicitarPosicionDeMemoria( uint32_t base,
		uint32_t offset, uint32_t tamanio) {


	Segmento * segmento = buscarSegmentoEnTabla( base);
	if( segmento == NULL){
		log_error( logger, "Segmento solicitado no valido");
		fprintf( mensajesUMV, "Segmento solicitado no valido\n");
		return -1;
	}

	if (chequearSegmentatiosFault(segmento, offset, tamanio)) {
		return -1;
	}
	imprimirBytes( base, offset, tamanio);


	return 1;

}

void imprimirBytes( uint32_t base, uint32_t offset, uint32_t tamanio){

	uint32_t alBuffer = base + offset;
	void * memoriaCorrida = memoria + alBuffer;

	uint32_t i, hastaLoQueDe= 0;
	unsigned char * mem = memoriaCorrida;

	printf("Direccion  | \t\t\tHex Dump\t\t\t|     ASCII\n");
	printf("---------------------------------------------------------------------------------\n");
	printf("    %05d  | ", alBuffer);

	for (i = 0; i < tamanio; i++) {

		unsigned char * posicion = memoriaCorrida + i;
		printf("%02X ", *posicion);
		hastaLoQueDe++;

		if( hastaLoQueDe != 16 && i == (tamanio - 1)){
			uint32_t blancos;
			for( blancos = 0; blancos < (16 - hastaLoQueDe); blancos++){
				printf("-- ");
				}
			mostrarCaracteres( hastaLoQueDe, mem);
		}
		if (hastaLoQueDe == 16 ) {

			mostrarCaracteres( hastaLoQueDe, mem);
			printf("\n---------------------------------------------------------------------------------\n");
			mem += hastaLoQueDe;
			alBuffer += hastaLoQueDe;
			hastaLoQueDe = 0;
			printf("    %05d  | ", alBuffer);
		}


	}
	printf("\n\n");
}

void mostrarCaracteres( uint32_t cantidad, unsigned char * mem){

	printf(" | ");
	uint32_t i;
	for (i = 0; i < cantidad; i++) {
		if (*(mem + i) == '\n' || *(mem + i) == '\t') {
			printf(" ");
		} else {
			printf("%c", *(mem + i));
		}
	}
}

uint32_t escribirPosicionDeMemoria( uint32_t base,
		uint32_t offset, uint32_t tamanio, uint32_t buffer[]) {


	Segmento * segmento = buscarSegmentoEnTabla( base);

	if( segmento == NULL){
			log_error( logger, "Segmento solicitado no valido");
			fprintf( mensajesUMV, "Segmento solicitado no valido\n");
			return -1;
		}

	if (chequearSegmentatiosFault(segmento, offset, tamanio)) {
		return -1;
	}

	uint32_t alBuffer = (base + offset);
	void * memoriaCorrida;
	memoriaCorrida = memoria + alBuffer;

	uint32_t i;
	for (i = 0; i < tamanio; i++) {
		memcpy(memoriaCorrida + i, &buffer[i], 1);
	}

	return 1;

}

bool chequearSegmentatiosFault(Segmento * segmento, uint32_t offset,
		uint32_t tamanio) {

	if (tamanio > (tamanioSegmento(segmento) - offset))
		return true;

	return false;
}

