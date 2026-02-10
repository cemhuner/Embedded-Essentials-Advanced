
#ifndef I2S_H_
#define I2S_H_

#include "stm32f4xx.h"
#include <stdint.h>

#define GPIOAEN	 (1<<0)

void DMA1_Stream3_SPI2_Config(void);
void SystemClock_Config(void);
void i2s_init();
void I2S_Config(void);
void process_audio_data(uint32_t* buffer, uint16_t size);


#endif /* I2S_H_ */
