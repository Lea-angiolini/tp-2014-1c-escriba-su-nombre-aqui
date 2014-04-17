#include "commons/log.h"
#include "Consola.h"
#include "commons/string.h"


char comandosBuffer[200];
extern t_log * logger;


void * iniciarConsola( void * params ){

	printf( "Consola de UMV, escriba-su-nombre-aqui\n");
	printf( "All rights reserved. 2014\n\n");

	while( 1 ){

		memset( &comandosBuffer, 0x0000, sizeof( comandosBuffer ) );
		printf( ">>> " );
		gets( &comandosBuffer );
		if( parsearComando( &comandosBuffer, logger ) == 0){
			break;
		}

	}

	log_info( logger, "\n\nFinalizando la consola ..." );

}




int parsearComando( char * comando, t_log * logger ){

	if ( string_starts_with( comando, "holi" ) ){
		printf("El comando es holi\n\n");
	}

	if ( string_starts_with( comando, "salir" ) ){
		return 0;
	}

	return 1;
}



