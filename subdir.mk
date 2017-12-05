################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/classloader.c \
../src/czvm.c \
../src/interpreter.c \
../src/list.c \
../src/log.c \
../src/mem.c \
../src/mymath.c \
../src/type.c 

OBJS += \
./src/classloader.o \
./src/czvm.o \
./src/interpreter.o \
./src/list.o \
./src/log.o \
./src/mem.o \
./src/mymath.o \
./src/type.o 

C_DEPS += \
./src/classloader.d \
./src/czvm.d \
./src/interpreter.d \
./src/list.d \
./src/log.d \
./src/mem.d \
./src/mymath.d \
./src/type.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


