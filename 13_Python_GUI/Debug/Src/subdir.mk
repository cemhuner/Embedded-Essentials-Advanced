################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/adxl345.c \
../Src/comm_drive.c \
../Src/help_func.c \
../Src/i2c.c \
../Src/i2c_dma.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/systick.c \
../Src/uart.c 

OBJS += \
./Src/adc.o \
./Src/adxl345.o \
./Src/comm_drive.o \
./Src/help_func.o \
./Src/i2c.o \
./Src/i2c_dma.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/systick.o \
./Src/uart.o 

C_DEPS += \
./Src/adc.d \
./Src/adxl345.d \
./Src/comm_drive.d \
./Src/help_func.d \
./Src/i2c.d \
./Src/i2c_dma.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/systick.d \
./Src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"C:/Users/orhan/STM32CubeIDE/workspace_1.13.1/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/orhan/STM32CubeIDE/workspace_1.13.1/chip_headers/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/adc.cyclo ./Src/adc.d ./Src/adc.o ./Src/adc.su ./Src/adxl345.cyclo ./Src/adxl345.d ./Src/adxl345.o ./Src/adxl345.su ./Src/comm_drive.cyclo ./Src/comm_drive.d ./Src/comm_drive.o ./Src/comm_drive.su ./Src/help_func.cyclo ./Src/help_func.d ./Src/help_func.o ./Src/help_func.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/i2c_dma.cyclo ./Src/i2c_dma.d ./Src/i2c_dma.o ./Src/i2c_dma.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/systick.cyclo ./Src/systick.d ./Src/systick.o ./Src/systick.su ./Src/uart.cyclo ./Src/uart.d ./Src/uart.o ./Src/uart.su

.PHONY: clean-Src

