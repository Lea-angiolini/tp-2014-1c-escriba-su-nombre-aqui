typedef struct {

	int cpuId;
	int pidProcesando;
	int socket;

} CPU;


void * fnNuevoCpu( void * socketPtr );
