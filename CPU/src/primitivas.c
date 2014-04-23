#include "commons/parser/parser.h"
#include "primitivas.h"
#include "commons/log.h"


extern t_log * logger;


//TODO crear ansisopKernel funciones
AnSISOP_funciones * crearAnSISOP_funciones() {

	AnSISOP_funciones * funciones = malloc(sizeof(AnSISOP_funciones));

	funciones->AnSISOP_definirVariable			= &definirVariable;
	funciones->AnSISOP_obtenerPosicionVariable	= &obtenerPosicionVariable;
	funciones->AnSISOP_dereferenciar			= &dereferenciar;
	funciones->AnSISOP_asignar					= &asignar;
	funciones->AnSISOP_obtenerValorCompartida	= &obtenerValorCompartida;
	funciones->AnSISOP_asignarValorCompartida	= &asignarValorCompartida;
	funciones->AnSISOP_irAlLabel				= &irAlLabel;
	funciones->AnSISOP_llamarSinRetorno 		= &llamarSinRetorno;
	funciones->AnSISOP_llamarConRetorno 		= &llamarConRetorno;
	funciones->AnSISOP_finalizar				= &finalizar;
	funciones->AnSISOP_retornar					= &retornar;
	funciones->AnSISOP_imprimir					= &imprimir;
	funciones->AnSISOP_imprimirTexto			= &imprimirTexto;
	funciones->AnSISOP_entradaSalida			= &entradaSalida;

	return funciones;

}



t_puntero definirVariable(t_nombre_variable identificador_variable) {
	log_debug( logger, "Llamada a definirVariable" );
	return 32;
}


t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	log_debug( logger, "Llamada a obtenerPosicionVariable" );
	return 32;
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {
	log_debug( logger, "Llamada a dereferenciar" );
	return 32;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {
	log_debug( logger, "Llamada a asignar" );

}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	log_debug( logger, "Llamada a obtenerValorCompartida" );
	return 32;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor) {
	log_debug( logger, "Llamada a asignarValorCompartida" );
	return 32;
}


void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta) {
	log_debug( logger, "Llamada a irAlLabel" );

}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	log_debug( logger, "Llamada a llamarSinRetorno" );

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	log_debug( logger, "Llamada a llamarConRetorno" );

}

void finalizar(void) {
	log_debug( logger, "Llamada a finalizar" );

}

void retornar(t_valor_variable retorno) {
	log_debug( logger, "Llamada a retornar" );

}

void imprimir(t_valor_variable valor_mostrar) {
	log_debug( logger, "Llamada a imprimir" );
}

void imprimirTexto(char* texto) {
	log_debug( logger, "Llamada a imprimirTexto" );
}

void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
	log_debug( logger, "Llamada a entradaSalida" );
}




























