

#include "systick.h"


#define SYSTICK_LOAD_VAL		16000  // cycles in a millisecond
#define CTRL_EN					(1<<0)
#define CTRL_CLKSRC				(1<<2)
#define CTRL_CNTFLAG			(1<<16)
#define ONE_SEC_LOAD			16000000
#define CTRL_TICKINT			(1<<1)


void systickDelayMs(int delay)
{
	SysTick->LOAD = SYSTICK_LOAD_VAL;
	SysTick->VAL = 0;
	SysTick->CTRL = CTRL_EN | CTRL_CLKSRC;

	for (int i=0; i<delay; i++)
	{
		while ((SysTick->CTRL & CTRL_CNTFLAG) == 0){} // countflag, 16k cycle tamamlandığı zaman "1" olur ve while döngüsü kırılır.
	}

	SysTick->CTRL = 0;

}

void systick_int_start(void)
{

	SysTick->LOAD = 16000 - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = CTRL_EN | CTRL_CLKSRC;
	SysTick->CTRL |= CTRL_TICKINT;

}

void systick_init(void)
{
	__disable_irq();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0x00FFFFFF;
	SysTick->VAL = 0;
	SysTick->CTRL = CTRL_EN | CTRL_CLKSRC;
	__enable_irq();
}

