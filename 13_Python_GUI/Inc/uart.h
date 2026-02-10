

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32f4xx.h"

void uart_tx_rx_init(void);
char UART_RX(void);
void UART_TX(int ch);

void UART_SEND(char str[]);
int _write(int file, char *ptr, int len);

#endif /* UART_H_ */
