
#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "stm32f4xx.h"

void systickDelayMs(int delay);

void systick_init(void);

void systick_int(unsigned short uart_2_mgr[]);

#endif /* SYSTICK_H_ */
