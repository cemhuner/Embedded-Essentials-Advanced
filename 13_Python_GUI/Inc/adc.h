/*
 * adc.h
 *
 *  Created on: Aug 15, 2025
 *      Author: orhan
 */

#ifndef ADC_H_
#define ADC_H_

void ADC1_Init(void);
void ADC1_ReadChannels(void);

void adc_irq(void);

#endif /* ADC_H_ */
