
#ifndef UART_DMA_H_
#define UART_DMA_H_

#include <stdint.h>
#include "stm32f4xx.h"

void uart_rx_tx_init(void);
void dma1_init(void);
void dma1_stream5_uart_rx_config(void);
void dma1_stream6_uart_tx_config(uint32_t send_msg, uint32_t msg_len);


#endif
