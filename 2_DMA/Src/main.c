#include "stm32f4xx.h"
#include <stdio.h>
#include "dma.h"
#include "uart.h"

#define BUFFER_SIZE			5

uint16_t sensor_data_arr[BUFFER_SIZE] = {892,731,1234,90,23};
uint16_t temp_data_arr[BUFFER_SIZE];

volatile uint8_t g_transfer_complete;

int main(void)
{
	g_transfer_complete = 0;

	uart_tx_rx_init();

	dma2_mem2mem_config();

	dma_transfer_init((uint32_t)sensor_data_arr, (uint32_t)temp_data_arr, BUFFER_SIZE);

	while(!g_transfer_complete){}



	for(int i=0; i<BUFFER_SIZE; i++)
	{
		printf("TEMP BUFFER[%d]: %d\n\r", i, temp_data_arr[i]);
	}

	g_transfer_complete = 0;

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
