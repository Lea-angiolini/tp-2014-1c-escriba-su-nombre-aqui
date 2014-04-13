#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"

int main(void) {
	t_config *config = config_create("ANSISOP_CONFIG");
	config_destroy(config);
	printf("asd");
	return EXIT_SUCCESS;
}
