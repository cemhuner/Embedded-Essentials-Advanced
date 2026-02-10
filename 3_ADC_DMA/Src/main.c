#include "stm32f4xx.h"
#include <stdio.h>
#include "dma.h"
#include "uart.h"
#include "dma_adc.h"

extern uint16_t adc_raw_data[10];

int main(void)
{


	uart_tx_rx_init();
	dma_adc_init();

	while(1)
	{
		printf("%d",adc_raw_data[10]);
	}
}


