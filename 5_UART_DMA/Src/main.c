#include "stm32f4xx.h"
#include <stdio.h>
#include "dma.h"
#include "uart.h"
#include "dma_adc.h"
#include "uart_dma.h"
#include <string.h>

extern char uart_data_buff[5];
extern uint8_t	g_rx_cmplt;
extern uint8_t	g_tx_cmplt;
char init_msg[120] = {'\0'};


int main(void)
{
	uart_rx_tx_init();
	dma1_init();
	dma1_stream5_uart_rx_config();

	dma1_stream6_uart_tx_config((uint32_t)init_msg , strlen(init_msg));

	while(!g_tx_cmplt){}

	while(1)
	{
		if(g_rx_cmplt)
		{
			sprintf(init_msg, "2", uart_data_buff);
			g_rx_cmplt = 0;
			g_tx_cmplt = 0;
			dma1_stream6_uart_tx_config((uint32_t)init_msg , strlen(init_msg));
			while(!g_tx_cmplt){}
		}


	}
}
