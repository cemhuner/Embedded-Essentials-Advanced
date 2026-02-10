#include "uart.h"



#define BAUDRATE    57600
#define FREQ        16000000
#define APB1_CLK	FREQ

#define GPIOAEN		(1<<0)
#define UART2EN		(1<<17)

#define CR1_TE		(1<<3)
#define CR1_RE		(1<<2)
#define CR1_UE		(1<<13)
#define SR_TXE		(1<<7)
#define SR_RXNE		(1<<5)


static void set_brr(USART_TypeDef *USARTx, uint32_t PeriClk, uint32_t BD);
static uint16_t compute_brr(uint32_t PeriClk, uint32_t BD);
void usart_write(int ch);

int __io_putchar(int ch)
{
	usart_write(ch);
	return ch;
}

void uart_tx_rx_init(void)
{
	RCC->AHB1ENR |= GPIOAEN;

	GPIOA->MODER &=~(1<<4);
	GPIOA->MODER |= (1<<5); // PA2 alternate function olarak ayarladık. çünkü alternate functionda (AF07) uart-tx olarak çalışıyor.

	GPIOA->MODER &=~(1<<6); // PA3 altenarte function olarak ayarlandı.
	GPIOA->MODER |= (1<<7);


	GPIOA->AFR[0] |= (1<<8); // PA2 pinini AF07 olarak ayarlamak için AFRL registerını 0111 (7) yaptık.
	GPIOA->AFR[0] |= (1<<9); // AFR'nin low kısmı PA0-PA7 kısmını, high kısmı PA8-PA15 kısmını tutar. low 0. indeks, high 1. indeks.
	GPIOA->AFR[0] |= (1<<10); // Biz PA2'yi ayarlayacağımız için low kısmı seçtik.
	GPIOA->AFR[0] &=~ (1<<11);

	GPIOA->AFR[0] |= (1<<12);
	GPIOA->AFR[0] |= (1<<13);
	GPIOA->AFR[0] |= (1<<14);
	GPIOA->AFR[0] &=~ (1<<15);


	RCC-> APB1ENR |= UART2EN;
	set_brr(USART2, APB1_CLK, BAUDRATE);

	USART2->CR1 = (CR1_TE | CR1_RE); // Transmitter enable 1 yaptık.
	USART2->CR1 |= CR1_UE; // Uart enable 1 yaptık. !! bitwise OR kullandık, mevcut CR1 üzerinden mask uyguladık.


}

void usart_write(int ch)
{
	while(!(USART2->SR & SR_TXE)){};
	USART2->DR = (ch & 0xFF); // veri uzunluğunu sınırladık. 0xFF = 11111111
}

char uart_read(void)
{
	while(!(USART2->SR & SR_RXNE)){};
	return USART2->DR;
}



static void set_brr(USART_TypeDef *USARTx, uint32_t PeriClk, uint32_t BD)
{
	USARTx->BRR = compute_brr(PeriClk, BD);
}


static uint16_t compute_brr(uint32_t PeriClk, uint32_t BD)
{
	return ((PeriClk + (BD/2))/BD);
}

