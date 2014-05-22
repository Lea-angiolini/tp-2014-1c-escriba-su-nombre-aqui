#include "config.h"

t_config *config;

AnSISOP_funciones * ansisop_funciones;
Stack * stackCache;

bool cargar_config(char *configFile)
{
	config = config_create(configFile);

	if( !validar_configuracion() ) {
		return false;
	}

	ansisop_funciones = crearAnSISOP_funciones();
	stackCache = new_Stack();

	return true;
}

bool validar_configuracion() {

	if (!config_has_property(config, "IPKERNEL")
			|| !config_has_property(config, "PUERTOKERNEL")
			|| !config_has_property(config, "IPUMV")
			|| !config_has_property(config, "PUERTOUMV"))
	{
		return false;
	}

	return true;
}

void destruir_config() {
	free(ansisop_funciones);
	free(stackCache);

	config_destroy(config);
}
