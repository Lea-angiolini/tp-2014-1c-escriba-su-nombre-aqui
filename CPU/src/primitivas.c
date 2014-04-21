#include "commons/parser/parser.h"



AnSISOP_funciones * crearAnSISOP_funciones(){

	AnSISOP_funciones * AnSISOP_funciones;

	AnSISOP_funciones->AnSISOP_definirVariable			= &definirVariable;
	AnSISOP_funciones->AnSISOP_obtenerPosicionVariable	= &obtenerPosicionVariable;
	AnSISOP_funciones->AnSISOP_dereferenciar			= &dereferenciar;
	AnSISOP_funciones->AnSISOP_asignar					= &asignar;
	AnSISOP_funciones->AnSISOP_obtenerValorCompartida	= &obtenerValorCompartida;
	AnSISOP_funciones->AnSISOP_asignarValorCompartida	= &asignarValorCompartida;
	AnSISOP_funciones->AnSISOP_irAlLabel				= &irAlLabel;
	AnSISOP_funciones->AnSISOP_llamarSinRetorno 		= &llamarSinRetorno;
	AnSISOP_funciones->AnSISOP_llamarConRetorno 		= &llamarConRetorno;
	AnSISOP_funciones->AnSISOP_finalizar				= &finalizar;
	AnSISOP_funciones->AnSISOP_retornar					= &retornar;
	AnSISOP_funciones->AnSISOP_imprimir					= &imprimir;
	AnSISOP_funciones->AnSISOP_imprimirTexto			= &imprimirTexto;
	AnSISOP_funciones->AnSISOP_entradaSalida			= &entradaSalida;

	return AnSISOP_funciones;

}



t_puntero definirVariable(t_nombre_variable identificador_variable){
	return 32;
}


t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){
	return 32;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){
	return 32;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor){

}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	return 32;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	return 32;
}


void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta){

}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){

}

void finalizar(void){

}

void retornar(t_valor_variable retorno){

}

void imprimir(t_valor_variable valor_mostrar){

}

void imprimirTexto(char* texto){

}

void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo){

}




























