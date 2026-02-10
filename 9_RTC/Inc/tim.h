
#ifndef TIM_H_
#define TIM_H_

#include <stdint.h>
#include "stm32f4xx.h"

void pseudo_delay(void);
void tim2_pa0_pa1_pwm_dutycycle(uint8_t ch, uint32_t dutycycle);
void tim2_pa0_pa1_pwm_init(void);

#endif
