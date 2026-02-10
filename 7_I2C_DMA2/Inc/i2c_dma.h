
#ifndef I2C_DMA_H_
#define I2C_DMA_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stddef.h>

void i2c1_gpio_init(void);
void i2c1_dma1_stream6_tx_init(void);
void i2c1_dma1_stream5_rx_init(void);
void i2c1_dma1_stream6_transfer(uint8_t *tx_msg, uint32_t len);
void i2c1_dma1_stream5_receive(uint8_t *rx_msg, uint32_t len);
void i2c_dma_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_read_buff, uint16_t num_bytes);
void i2c_dma_write(uint8_t slave_addr, uint8_t *p_write_buff, uint16_t num_bytes);


#endif
