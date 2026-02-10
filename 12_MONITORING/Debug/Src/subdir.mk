################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/dma.c \
../Src/dma_adc.c \
../Src/i2c_dma.c \
../Src/main.c \
../Src/mpu9250.c \
../Src/spi_dma.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/uart.c 

OBJS += \
./Src/dma.o \
./Src/dma_adc.o \
./Src/i2c_dma.o \
./Src/main.o \
./Src/mpu9250.o \
./Src/spi_dma.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/uart.o 

C_DEPS += \
./Src/dma.d \
./Src/dma_adc.d \
./Src/i2c_dma.d \
./Src/main.d \
./Src/mpu9250.d \
./Src/spi_dma.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"C:/Users/orhan/STM32CubeIDE/workspace_1.13.1/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/orhan/STM32CubeIDE/workspace_1.13.1/chip_headers/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/dma.cyclo ./Src/dma.d ./Src/dma.o ./Src/dma.su ./Src/dma_adc.cyclo ./Src/dma_adc.d ./Src/dma_adc.o ./Src/dma_adc.su ./Src/i2c_dma.cyclo ./Src/i2c_dma.d ./Src/i2c_dma.o ./Src/i2c_dma.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/mpu9250.cyclo ./Src/mpu9250.d ./Src/mpu9250.o ./Src/mpu9250.su ./Src/spi_dma.cyclo ./Src/spi_dma.d ./Src/spi_dma.o ./Src/spi_dma.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/uart.cyclo ./Src/uart.d ./Src/uart.o ./Src/uart.su

.PHONY: clean-Src

