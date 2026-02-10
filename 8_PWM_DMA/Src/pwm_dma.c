
#include "pwm_dma.h"

#define GPIOA_EN			(1<<0)
#define TIM2_EN				(1<<0)
#define CR1_CEN				(1<<0)

#define DIER_CC1DE			(1<<9)
#define DIER_CC2DE			(1<<10)
#define DIER_UDE			(1<<8)
#define DIER_TDE			(1<<14)
#define LOOKUP_TABLE_LEN	200

uint32_t ch1_duty_cycle_lookup[LOOKUP_TABLE_LEN]={

};

void tim2_pa0_pa1_pwm_dma_init(void)
{
	/* GPIO CONFIGURATIONS */

	RCC->AHB1ENR |= GPIOA_EN;

	GPIOA->MODER &=~ (1<<0);
	GPIOA->MODER |=  (1<<1);

	GPIOA->MODER &=~ (1<<2);
	GPIOA->MODER |=  (1<<3);

	GPIOA->AFR[0] |=  (1<<0);
	GPIOA->AFR[0] &=~ (1<<1);
	GPIOA->AFR[0] &=~ (1<<2);
	GPIOA->AFR[0] &=~ (1<<3);

	GPIOA->AFR[0] |=  (1<<4);
	GPIOA->AFR[0] &=~ (1<<5);
	GPIOA->AFR[0] &=~ (1<<6);
	GPIOA->AFR[0] &=~ (1<<7);

	/* TIMER CONFIGURATIONS */

	RCC->APB1ENR |= TIM2_EN;

	TIM2->PSC = 16 - 1;  // 16MHz/16 = 1MHz

	TIM2->ARR = 20000 - 1; // 1MHz/20k = 50 Hz

	TIM2->CNT = 0;

	TIM2->CCMR1 &=~ (1<<4);
	TIM2->CCMR1 |= (1<<5);
	TIM2->CCMR1 |= (1<<6);

	TIM2->CCMR1 &=~ (1<<12);
	TIM2->CCMR1 |= (1<<13);
	TIM2->CCMR1 |= (1<<14);

	TIM2->DIER |= DIER_CC1DE | DIER_CC2DE | DIER_UDE | DIER_TDE;

	TIM2->CCER |= (1<<0);

	TIM2->CCER |= (1<<4);

	TIM2->CR1 |= CR1_CEN;

	/* DMA CONFIGURATIONS
	 *
	 * DMA1_Stream5 Channel 3 - TIM2_CH1
	 * DMA1_Stream6 Channel 3 - TIM2_CH2
	 *
	 * PA0: TIM2_CH1
	 * PA1: TIM2_CH2
	 */

	RCC->AHB1ENR |= DMA1_EN; // Clock access to DMA1 module

	DMA1_Stream5->CR = 0;

	while((DMA1_Stream5->CR & DMA_SCR_EN)){}

	DMA1_Stream6->CR |= (1<<25);
	DMA1_Stream6->CR |= (1<<26);
	DMA1_Stream6->CR &=~ (1<<27); // Channel 3

	DMA1_Stream5->CR |= DMA_SCR_MINC;

	DMA1_Stream6->CR &=~ (1<<13);
	DMA1_Stream6->CR |=  (1<<14); // MEM SIZE 32 BIT

	DMA1_Stream6->CR &=~ (1<<11);
	DMA1_Stream6->CR |=  (1<<12); // MEM SIZE 32 BIT

	DMA1_Stream5->CR &=~ (1<<6);
	DMA1_Stream5->CR |=  (1<<7);

	DMA1_Stream5->NDTR = (uint16_t)LOOKUP_TABLE_LEN;
	DMA1_Stream5->PAR = (uint32_t)(&TIM2->CCR1);

}

