################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/so-commons-library/src/commons/collections/dictionary.c \
/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/so-commons-library/src/commons/collections/list.c \
/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/so-commons-library/src/commons/collections/queue.c 

OBJS += \
./src/commons/collections/dictionary.o \
./src/commons/collections/list.o \
./src/commons/collections/queue.o 

C_DEPS += \
./src/commons/collections/dictionary.d \
./src/commons/collections/list.d \
./src/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/commons/collections/dictionary.o: /home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/so-commons-library/src/commons/collections/dictionary.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/Programa/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/commons/collections/list.o: /home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/so-commons-library/src/commons/collections/list.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/Programa/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/commons/collections/queue.o: /home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/so-commons-library/src/commons/collections/queue.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/Programa/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


