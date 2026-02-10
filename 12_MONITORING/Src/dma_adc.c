
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


#define TIM2_EN 		(1<<0)


uint16_t adc_raw_data[10];


void dma_tim_adc_init(void)
{
	/*GPIO configurations*/

	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER |= (1<<0);
	GPIOA->MODER |= (1<<1); // Set PA0 analog mode

	/*ADC configurations*/

	RCC->APB2ENR |= ADC1EN;

	ADC1->CR2 = CR2_DMA | CR2_DDS; // Enable dma access over ADC1

	ADC1->CR2 |= (1<<28);
	ADC1->CR2 &=~ (1<<29); // External trigger on rizzing edge

	ADC1->CR2 &=~ (1<<24);
	ADC1->CR2 |= (1<<25);
	ADC1->CR2 |= (1<<26);
	ADC1->CR2 &=~ (1<<27); // External event as TIMER2 TRGO



	/* DMA configurations */

	RCC->AHB1ENR |= DMA2_EN; // Enable clock access to DMA module

	DMA2_Stream0->CR &=~ DMA_SCR_EN; // Disable DMA module

	while((DMA2_Stream0->CR) & DMA_SCR_EN){} // Wait until DMA module disabled

	DMA2_Stream0->CR |= DMA_SCR_CIRC; // Enable circular mode

	DMA2_Stream0->CR |= (1<<11);
	DMA2_Stream0->CR &=~ (1<<12); // PSIZE to half-word (16 bit)

	DMA2_Stream0->CR |= (1<<13);
	DMA2_Stream0->CR &=~ (1<<14); // MEMSIZE to half-word (16 bit)

	DMA2_Stream0->CR |= DMA_SCR_MINC; // Memory increment, doesn't have to increment PINC since ADC has only static DR

	DMA2_Stream0->CR |= DMA_SCR_TCIE;
	DMA2_Stream0->CR |= DMA_SCR_TEIE;

	DMA2_Stream0->PAR |= (uint32_t)(&(ADC1->DR));

	DMA2_Stream0->M0AR = (uint32_t)(&adc_raw_data);

	DMA2_Stream0->NDTR = (uint16_t)10; // 10 samples

	NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	/*TIMER configurations*/

	RCC->APB1ENR |= TIM2_EN;

	TIM2->PSC = 16000 - 1; // 16M/16000 = 1000

	TIM2->ARR = 10 - 1; // 1000/10 = 100Hz

	TIM2->CR2 &=~ (1<<4);
	TIM2->CR2 |= (1<<5);
	TIM2->CR2 &=~ (1<<6); // MMS update mode

	DMA2_Stream0->CR |= DMA_SCR_EN; // Enable DMA

	ADC1->CR2 |= CR2_ADCON; // ADC enabled

	TIM2->CR1 |= (1<<0); // TIMER enabled

}






