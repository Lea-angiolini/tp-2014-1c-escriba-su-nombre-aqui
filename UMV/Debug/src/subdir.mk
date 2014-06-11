################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CPU.c \
../src/Consola.c \
../src/Kernel.c \
../src/Programa.c \
../src/Segmento.c \
../src/UMV.c \
../src/memoria.c \
../src/mocks.c 

OBJS += \
./src/CPU.o \
./src/Consola.o \
./src/Kernel.o \
./src/Programa.o \
./src/Segmento.o \
./src/UMV.o \
./src/memoria.o \
./src/mocks.o 

C_DEPS += \
./src/CPU.d \
./src/Consola.d \
./src/Kernel.d \
./src/Programa.d \
./src/Segmento.d \
./src/UMV.d \
./src/memoria.d \
./src/mocks.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/commons" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


