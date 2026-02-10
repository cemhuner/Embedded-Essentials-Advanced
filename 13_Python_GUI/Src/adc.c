

#include "stm32f4xx.h"
#include "adc.h"

#define ADC1EN			(1<<8)
#define ADC_CH1			(1<<0)
#define ADC_LEN_1		(0x00)
#define CR2_ADON		(1<<0)
#define CR2_SWSTART		(1<<30)
#define SR_EOC			(1<<1)
#define CR2_CONT		(1<<1)

volatile uint16_t adc_buffer[2];

void ADC1_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= (3 << (0*2)) | (3 << (1*2));

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC->CCR = 0;

    ADC1->CR1 = 0;
    ADC1->CR2 = 0;

    ADC1->CR1 |= ADC_CR1_SCAN;
    //ADC1->CR2 |= ADC_CR2_CONT;
    ADC1->CR2 |= ADC_CR2_EOCS;   // <<< her conversion sonunda EOC set edilir

    ADC1->SQR1 = (1 << 20);
    ADC1->SQR3 = (0 << 0) | (1 << 5);

    ADC1->SMPR2 |= (7 << (3*0)) | (7 << (3*1));

    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}
void ADC1_ReadChannels(void) {
    // Start conversion sequence
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Wait for first conversion (CH0)
    while(!(ADC1->SR & ADC_SR_EOC));
    adc_buffer[0] = ADC1->DR;

    // Wait for second conversion (CH1)
    while(!(ADC1->SR & ADC_SR_EOC));
    adc_buffer[1] = ADC1->DR;
}






