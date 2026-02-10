
#include "tim.h"

#define GPIOA_EN			(1<<0)
#define TIM2_EN				(1<<0)
#define CR1_CEN				(1<<0)

void tim2_pa0_pa1_pwm_init(void)
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

	TIM2->PSC = 16 - 1 ;

	TIM2->ARR = 20000 - 1;

	TIM2->CNT = 0;

	TIM2->CCMR1 &=~ (1<<4);
	TIM2->CCMR1 |= (1<<5);
	TIM2->CCMR1 |= (1<<6);

	TIM2->CCMR1 &=~ (1<<12);
	TIM2->CCMR1 |= (1<<13);
	TIM2->CCMR1 |= (1<<14);

	TIM2->CCER |= (1<<0);

	TIM2->CCER |= (1<<4);

	TIM2->CR1 |= CR1_CEN;
}

void tim2_pa0_pa1_pwm_dutycycle(uint8_t ch, uint32_t dutycycle)
{
	switch(ch)
	{
		case 1:
			TIM2->CCR1 = dutycycle;
			break;
		case 2:
			TIM2->CCR2 = dutycycle;
			break;
		default:
			break;
	}
}

void pseudo_delay(void)
{
	for(int i = 0; i<5000; i++){}
}






