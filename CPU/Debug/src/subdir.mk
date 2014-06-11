################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CPU.c \
../src/config.c \
../src/ejecucion.c \
../src/kernel.c \
../src/mocks.c \
../src/primitivas.c \
../src/stack.c \
../src/umv.c 

OBJS += \
./src/CPU.o \
./src/config.o \
./src/ejecucion.o \
./src/kernel.o \
./src/mocks.o \
./src/primitivas.o \
./src/stack.o \
./src/umv.o 

C_DEPS += \
./src/CPU.d \
./src/config.d \
./src/ejecucion.d \
./src/kernel.d \
./src/mocks.d \
./src/primitivas.d \
./src/stack.d \
./src/umv.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


