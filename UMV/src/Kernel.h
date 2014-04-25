#ifndef KERNEL_H_
#define KERNEL_H_


typedef struct {

	int socket;

} Kernel ;


void * fnKernelConectado( void * socketPtr );
int procesarMenssajeKernel( Kernel * kernel, char * mensaje );




#endif /* KERNEL_H_ */
