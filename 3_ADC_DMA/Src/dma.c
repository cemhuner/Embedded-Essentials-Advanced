#include "dma.h"

#define DMA2_EN 			(1<<22)
#define DMA_SCR_EN			(1<<0)  // DMA stream configuration register
#define DMA_SCR_MINC		(1<<10)
#define DMA_SCR_PINC		(1<<9)
#define DMA_SCR_TCIE		(1<<4) // Transfer complete interrupt
#define DMA_SCR_TEIE		(1<<2) // Transfer error interrupt
#define DMA2_SFCR_DMDIS		(1<<2)


void dma2_mem2mem_config(void)
{
	RCC->AHB1ENR |= DMA2_EN; // Enable access to DMA module

	DMA2_Stream0->CR = 0; // Disable DMA stream

	while ((DMA2_Stream0->CR & DMA_SCR_EN)){} // Wait until stream is disabled

	DMA2_Stream0->CR |= (1<<13);
	DMA2_Stream0->CR &=~ (1<<14); // MEMSIZE to half-word (16 bit)

	DMA2_Stream0->CR |= (1<<11);
	DMA2_Stream0->CR &=~ (1<<12); // PSIZE to half-word (16 bit)

	DMA2_Stream0->CR |= DMA_SCR_MINC; // Memory increment
	DMA2_Stream0->CR |= DMA_SCR_PINC; // Peripheral increment

	DMA2_Stream0->CR &=~ (1<<6);
	DMA2_Stream0->CR |= (1<<7); // MEM2MEM enable

	DMA2_Stream0->FCR |= DMA2_SFCR_DMDIS; // Disable direct mode

	DMA2_Stream0->FCR |= (1<<0);
	DMA2_Stream0->FCR |= (1<<1); // Set DMA threshold

	NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

void dma_transfer_init(uint32_t src_buff, uint32_t dest_buff, uint32_t len)
{
	DMA2_Stream0->PAR = src_buff;

	DMA2_Stream0->M0AR = dest_buff;

	DMA2_Stream0->NDTR = len;

	DMA2_Stream0->CR |= DMA_SCR_EN;

}






