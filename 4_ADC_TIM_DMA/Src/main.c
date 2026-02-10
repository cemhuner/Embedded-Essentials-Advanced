#include "stm32f4xx.h"
#include <stdio.h>
#include "dma.h"
#include "uart.h"
#include "dma_adc.h"


extern uint16_t adc_raw_data[10];

volatile uint8_t g_transfer_complete;

int main(void)
{
	g_transfer_complete = 0;

	uart_tx_rx_init();

	dma_tim_adc_init();

	while(1)
	{

	}
}


void DMA2_Stream0_IRQHandler(void)
{
	if((DMA2->LISR) & LISR_TCIF0)
	{
		g_transfer_complete = 1;

		DMA2->LIFCR |= LIFCR_CTCIF0;
	}

	if((DMA2->LISR) & LISR_TEIF0)
	{
		DMA2->LIFCR |= LIFCR_CTEIF0;
	}


}
