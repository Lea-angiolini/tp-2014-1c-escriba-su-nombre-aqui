################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel.c \
../src/colas.c \
../src/config.c \
../src/io.c \
../src/pcp.c \
../src/plp.c \
../src/systemcalls.c 

OBJS += \
./src/Kernel.o \
./src/colas.o \
./src/config.o \
./src/io.o \
./src/pcp.o \
./src/plp.o \
./src/systemcalls.o 

C_DEPS += \
./src/Kernel.d \
./src/colas.d \
./src/config.d \
./src/io.d \
./src/pcp.d \
./src/plp.d \
./src/systemcalls.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-escriba-su-nombre-aqui/commons" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


