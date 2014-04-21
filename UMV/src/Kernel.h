typedef struct {

	int socket;

} Kernel ;


void * fnKernelConectado( void * socketPtr );
int procesarMenssajeKernel( Kernel * kernel, char * mensaje );
