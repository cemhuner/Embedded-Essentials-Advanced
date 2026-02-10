
#include "dma_adc.h"

#define GPIOAEN			(1<<0)
#define ADC1EN			(1<<8)
#define CR1_SCAN		(1<<8) // Scan-mode since we have 2 conversions
#define CR2_DMA			(1<<8) // DMA access for ADC
#define CR2_DDS			(1<<9) // DMA disable selection
#define CR2_CONT		(1<<1) // Continuous mode
#define CR2_ADCON		(1<<0) // ADC-ON
#define CR2_SWSTART		(1<<30)


#define DMA2_EN 			(1<<22)
#define DMA_SCR_EN			(1<<0)  // DMA stream configuration register
#define DMA_SCR_MINC		(1<<10)
#define DMA_SCR_PINC		(1<<9)
#define DMA_SCR_TCIE		(1<<4) // Transfer complete interrupt
#define DMA_SCR_TEIE		(1<<2) // Transfer error interrupt
#define DMA2_SFCR_DMDIS		(1<<2)
#define DMA_SCR_CIRC		(1<<8)


uint16_t adc_raw_data[2];


void dma_adc_init(void)
{
	/*GPIO configurations*/

	RCC->AHB1ENR |= GPIOAEN;

	GPIOA->MODER |= (1<<0);
	GPIOA->MODER |= (1<<1);

	GPIOA->MODER |= (1<<2);
	GPIOA->MODER |= (1<<3); // Set PA0 & PA1 analog mode


	/*ADC configurations*/

	RCC->APB2ENR |= ADC1EN;

	ADC1->SQR1 |= (1<<20);
	ADC1->SQR1 &=~ (1<<21);
	ADC1->SQR1 &=~ (1<<22);
	ADC1->SQR1 &=~ (1<<23); // Conversion length selected as 2 (0b0001)

	ADC1->SQR3 = (0<<0) | (1<<5); // PA0 & PA1 selected as 1st and 2nd conversions respectively
	ADC1->CR1 = CR1_SCAN;  // Enable scan mode
	ADC1->CR2 |= CR2_CONT | CR2_DMA | CR2_DDS;


	RCC->AHB1ENR |= DMA2_EN; // Enable clock access to DMA module

	DMA2_Stream0->CR &=~ DMA_SCR_EN; // Disable DMA module

	while((DMA2_Stream0->CR) & DMA_SCR_EN){} // Wait until DMA module disabled

	DMA2_Stream0->CR |= DMA_SCR_CIRC; // Enable circular mode

	DMA2_Stream0->CR |= (1<<11);
	DMA2_Stream0->CR &=~ (1<<12); // PSIZE to half-word (16 bit)

	DMA2_Stream0->CR |= (1<<13);
	DMA2_Stream0->CR &=~ (1<<14); // MEMSIZE to half-word (16 bit)

	DMA2_Stream0->CR |= DMA_SCR_MINC; // Memory increment, doesn't have to increment PINC since ADC has only static DR

	DMA2_Stream0->PAR |= (uint32_t)(&(ADC1->DR));

	DMA2_Stream0->M0AR = (uint32_t)(&adc_raw_data);

	DMA2_Stream0->NDTR = 2; // 2 data set

	DMA2_Stream0->CR |= DMA_SCR_EN; // Enable DMA

	ADC1->CR2 |= CR2_ADCON;

	ADC1->CR2 |= CR2_SWSTART;
}
