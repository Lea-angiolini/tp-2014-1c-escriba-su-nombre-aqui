#include "Consola.h"
#include "Segmento.h"
#include "Programa.h"

#include "memoria.h"
#include "config.h"

extern t_log * logger;
extern pthread_rwlock_t lockEscrituraLectura;
extern t_list * tabla_segmentos;

char comandosBuffer[200];

void * iniciarConsola(void * params) {

	printf("Consola de UMV, escriba-su-nombre-aqui\n");
	printf("All rights reserved. 2014\n\n");

	while (1) {

		char comando;
		printf("¿Que operacion desea realizar?\n");
		printf("Ingrese por teclado la letra de la operacion a realizar\n");
		printf("a - Operaciones con segmentos\n");
		printf("b - Modificar retardo ante una solicitud\n");
		printf("c - Modificar algoritmo de modificacion de segmentos\n");
		printf("d - Compactar tabla de segmentos\n");
		printf("e - Generar dump\n");

		comando = getchar();
		while (getchar() != '\n')
			;

		switch (comando) {
		case 'a':
			operacionesConSegmentos();
			break;
		case 'b':
			modificarRetardoUMV();
			break;
		case 'c':
			modificarAlgoCreacionSegmentos();
			break;
		case 'd':
			compactar();
			break;
		case 'e':
			generarDump();
			break;
		default:
			log_error(logger, "El comando ingresado no es valido");
			break;
		}
	}
	return NULL ;
}

void operacionesConSegmentos() {

	char operacion;

	printf("¿Que operacion desea realizar?\n");
	printf("a - Solicitar una posicion de memoria\n");
	printf("b - Escribir una posicion de memoria\n");
	printf("c - Crear un Segmento\n");
	printf("d - Destruir un Segmento\n");

	operacion = getchar();
	while (getchar() != '\n')
		;

	switch (operacion) {

	case 'a':
		requisitosOperacionSegmento(operacion);
		break;
	case 'b':
		requisitosOperacionSegmento(operacion);
		break;
	case 'c':
		printf("Ingrese el tamanio del segmento a crear\n");
		uint32_t tamanio;
		scanf("%d", &tamanio);
		while (getchar() != '\n')
			;
		if(tamanio > memoriaLibre()){
			printf("El tamaño solicitado excede el espacio libre en memoria");
		}else{
			crearSegmento(tamanio);

		}

		break;

	case 'd':
		printSegmentos( tabla_segmentos);
		printf("Ingrese el ID del segmento a destruir");
		uint32_t idSeg;
		scanf("%d", &idSeg);
		while (getchar() != '\n');
		pthread_rwlock_wrlock(&lockEscrituraLectura);
		Segmento * segmento = buscarSegmentoEnTabla( idSeg);
		borrarSegmento( segmento);
		pthread_rwlock_unlock(&lockEscrituraLectura);
		break;

	default:
		printf("El comando ingresado es invalido\n");
	}

}

void requisitosOperacionSegmento(char operacion) {

	usleep( retardoUMV * 1000);

	pthread_rwlock_wrlock(&lockEscrituraLectura);


	printSegmentos( tabla_segmentos);

	printf("Ingrese la base del segmento a solicitar\n");
	uint32_t base;
	scanf("%d", &base);
	while (getchar() != '\n')
		;

	printf("Ingrese un offset al segmento\n");
	uint32_t offset;
	scanf("%d", &offset);
	while (getchar() != '\n')
		;

	printf("Ingrese el tamanio que desea solicitar\n");
	uint32_t tamanio;
	scanf("%d", &tamanio);
	while (getchar() != '\n')
		;

		switch (operacion) {
		case 'a':solicitarPosicion(base, offset, tamanio);
				break;

		case 'b': escribirPosicion(base, offset, tamanio);
				break;

		default: break;
		}

	pthread_rwlock_unlock(&lockEscrituraLectura);
}


void solicitarPosicion( uint32_t base, uint32_t offset,
		uint32_t tamanio) {

	uint32_t solicitud = solicitarPosicionDeMemoria( base, offset,
			tamanio);

	if (solicitud == -1)
		printf("No se ha podido realizar la lectura de memoria\n");

}

void escribirPosicion( uint32_t base, uint32_t offset,
		uint32_t tamanio) {

	printf(
			"Ingrese el buffer que desea escribir, presionando ENTER entre cada byte\n");
	uint32_t buffer[tamanio];

	uint32_t i;
	for (i = 0; i < tamanio; i++) {
		scanf("%d", &buffer[i]);
		while (getchar() != '\n')
			;
	}

	uint32_t escritura = escribirPosicionDeMemoria( base, offset,
			tamanio, buffer);

	if (escritura == -1)
		printf("No se ha podido realizar la escritura de memoria\n");
}

void modificarRetardoUMV() {

	printf("Ingrese el retardo que quiere modificar en milisegundos \n");
	scanf("%d", &retardoUMV);
	while (getchar() != '\n')
		;
	printf("Este es el nuevo retardo %d\n", retardoUMV);
}

void modificarAlgoCreacionSegmentos() {

	printf("El algoritmo actual de creacion de segmentos es ");
	if( modoActualCreacionSegmentos == WORSTFIT){
		printf("WORST-FIT\n");
	} else printf("FIRST-FIT\n");

	printf("Desea cambiarlo? \ns- Si \nn- No\n");
	char cambio;
	cambio = getchar();
	while(getchar()!='\n');
	if( cambio == 's'){
		if( modoActualCreacionSegmentos == WORSTFIT){
			modoActualCreacionSegmentos = FIRSTFIT;
		} else modoActualCreacionSegmentos = WORSTFIT;
	}

	printf("El algoritmo actual de creacion de segmentos es ");
	if( modoActualCreacionSegmentos == WORSTFIT){
		printf("WORS-FIT\n");
	} else printf("FIRST-FIT\n");

}

void generarDump() {
	char reporte;

	printf("Indique que reporte quiere\n");
	printf("a-Tabla de segmentos de todos los procesos\n");
	printf("b-Tabla de segmento de un proceso en particular\n");
	printf("c-Detalle de la memoria principal\n");
	printf("d-Contenido de la memoria principal\n");

	reporte = getchar();
	while (getchar() != '\n')
		;

	switch (reporte) {
	case 'a':
		printSegmentosPorPrograma();
		break;
	case 'b':
		buscarProgramaEImprimirSegmentos();
		break;
	case 'c':
		ordenarTablaSegmentos();
		printSegmentos(tabla_segmentos);
		break;
	case 'd': imprimirMemoria();
				break;
	default:
		log_error(logger, "El comando ingresado es invalido");

	}
}

void imprimirMemoria(){
	uint32_t offset, tamanio;
	printf("Indique un offset desde el comienzo de la memoria");
	scanf("%d", &offset);
	while( getchar() != '\n');
	printf("Indique una cantidad de bytes");
	scanf("%d", &tamanio);
	while( getchar() != '\n');

	if( memoria_size < (tamanio + offset)){
		log_error( logger, "Se ha producido Segmentation Fault a causa de indicar una cantidad de bytes que sobrepasa el tamaño de la memoria");
		}else{
		imprimirBytes( 0, offset, tamanio);
		}
}

void printSegmentosHeaders() {
	printf("\n");
	printf("\t\t|  Inicio Real  |   Fin  Real   |     Tamaño\n");
	printf("----------------|---------------|---------------|-------------|\n");
}

void imprimirSegmentosDe(Programa *programa) {
	printf("Programa: %d \n", programa->pid);
	printSegmentosHeaders();
	printSegmento(programa->stack);
	printSegmento(programa->script);
	printSegmento(programa->etiquetas);
	printSegmento(programa->instrucciones);
}

void printSegmentosPorPrograma() {
	uint32_t tamanioProgramas;
	tamanioProgramas = list_size(programas);
	int i = 0;
	for (i = 0; i <= tamanioProgramas - 1; i++) {
		Programa * programa = list_get(programas, i);
		imprimirSegmentosDe(programa);
	}

}

void buscarProgramaEImprimirSegmentos() {
	uint32_t pid;
	Programa *programaAImprimir;

	printf("Ingrese el id del programa para imprimir sus segmentos\n");

	scanf("%d\n\n", &pid);
	while (getchar() != '\n')
		;

	programaAImprimir = buscarPrograma(pid);
	imprimirSegmentosDe(programaAImprimir);

}

void printSegmentos(t_list * segmentos) {
	ordenarTablaSegmentos();
	printSegmentosHeaders();

	int i = 0;
	uint32_t cont = 0;
	for (i = 0; i < list_size(segmentos); i++) {
		Segmento * segmento = (Segmento *) list_get(segmentos, i);
		if (segmento->inicioReal != cont) {
			printEspacioLibre(cont, segmento->inicioReal - 1);
			cont = segmento->finReal + 1;
			printf("----------------|---------------|-----------------------------|\n");
		}

		printSegmento(segmento);
		cont = segmento->finReal + 1;
		printf("----------------|---------------|-----------------------------|\n");

	}
	if (cont < (memoria_size - 1)){
		printEspacioLibre(cont, (memoria_size - 1));
		printf("--------------------------------------------------------------|\n");
	}
}

void printSegmento(Segmento * segmento) {
	printf("\t%04d\t|\t%04d\t|\t%04d\t|\t%05d |\n", segmento->id, segmento->inicioReal, segmento->finReal,
	tamanioSegmento(segmento));

}

void printEspacioLibre(uint32_t inicioEspacio, uint32_t finEspacio) {
	printf("\tLibre\t|\t%04d\t|\t%04d\t|\t%05d\n", inicioEspacio, finEspacio,
			finEspacio - inicioEspacio + 1);
}

