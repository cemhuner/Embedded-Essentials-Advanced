
#include "spi_dma.h"

#define GPIOA_EN			(1<<0)
#define SPI1_EN				(1<<12)
#define DMA2_EN 			(1<<22) // DMA Enable


#define CR1_SSM				(1<<9) // SPI Control Register-1 Sotfware Slave Management
#define CR1_SSI				(1<<8) // SPI Control Register-1 Internal Slave Select
#define CR1_MSTR			(1<<2) // SPI Control Register-1 Master-Slave Selection
#define CR1_CPOL			(1<<1) // SPI Control Register-1 Clock Polarity
#define CR1_CPHA			(1<<0) // SPI Control Register-1 Clock Phase
#define CR2_TXDMAEN			(1<<1) // SPI Control Register-2 TX Buffer DMA Enable
#define CR2_RXDMAEN			(1<<0) // SPI Control Register-2 RX Buffer DMA Enable
#define CR1_SPE				(1<<6) // SPI Control Register-2 SPI Enable


#define DMA_SCR_EN			(1<<0) 	// DMA1 Stream Configuration Register Stream Enable
#define DMA_SCR_CIRC		(1<<8) 	// DMA1 Stream Configuration Register Circular Mode Enable
#define DMA_SCR_MINC		(1<<10) // DMA1 Stream Configuration Register Memory Increment
#define DMA_SCR_PINC		(1<<9) 	// DMA1 Stream Configuration Register Peripheral Increment
#define DMA_SCR_TCIE		(1<<4) 	// DMA1 Stream Configuration Register Transfer Complete Interrupt Enable
#define DMA_SCR_TEIE		(1<<2) 	// DMA1 Stream Configuration Register Transfer Error Interrupt Enable
#define DMA2_SFCR_DMDIS		(1<<2)	// DMA2 FIFO Control Register Direct Mode Disable

#define LIFCR_CTCIF2		(1<<21) // Clear Transfer Complete Interrupt Flag
#define LIFCR_CTEIF2		(1<<19) // Clear Transfer Error Interrupt Flag

#define LIFCR_CTCIF3		(1<<27) // Clear Transfer Complete Interrupt Flag
#define LIFCR_CTEIF3		(1<<25) // Clear Transfer Error Interrupt Flag


void spi1_dma_init(void)
{
	RCC->AHB1ENR |= GPIOA_EN;

	GPIOA->MODER &=~ (1<<10);
	GPIOA->MODER |=  (1<<11); // PA5 Alternate function

	GPIOA->MODER &=~ (1<<12);
	GPIOA->MODER |=  (1<<13); // PA6 Alternate function

	GPIOA->MODER &=~ (1<<14);
	GPIOA->MODER |=  (1<<15); // PA7 Alternate function

	GPIOA->AFR[0] |= (1<<20);
	GPIOA->AFR[0] &=~ (1<<21);
	GPIOA->AFR[0] |= (1<<22);
	GPIOA->AFR[0] &=~ (1<<23); // PA5 AF05

	GPIOA->AFR[0] |= (1<<24);
	GPIOA->AFR[0] &=~ (1<<25);
	GPIOA->AFR[0] |= (1<<26);
	GPIOA->AFR[0] &=~ (1<<27); // PA6 AF05

	GPIOA->AFR[0] |= (1<<28);
	GPIOA->AFR[0] &=~ (1<<29);
	GPIOA->AFR[0] |= (1<<30);
	GPIOA->AFR[0] &=~ (1<<31); // PA7 AF05

	RCC->APB2ENR |= SPI1_EN; // Enable SPI module clock access

	SPI1->CR1 |= CR1_SSM | CR1_SSI; // Select Internal slave mode

	SPI1->CR1 |= CR1_MSTR; // Select Master-Slave Mode

	SPI1->CR1 |= SPI_CR1_BR_0 | CR1_CPHA | CR1_CPOL;

	SPI1->CR1 |= (1<<3);
	SPI1->CR1 &=~ (1<<4);
	SPI1->CR1 &=~ (1<<5); // 001 means BAUDRATE should be 16/4 = 4MHz

	SPI1->CR2 |= CR2_TXDMAEN | CR2_RXDMAEN; // DMA Tx and Rx enable

	SPI1->CR1 |= CR1_SPE; // SPI enable
}

void dma2_stream3_spi_tx_init(void)
{
	RCC->AHB1ENR |= DMA2_EN;

	DMA2_Stream3->CR = 0;

	while((DMA2_Stream3->CR) & DMA_SCR_EN){}

	DMA2_Stream3->CR |= DMA_SCR_MINC; // DMA Memory increment (won't configure PINC since USART Data register address is static)

	DMA2_Stream3->CR |= (1<<25);
	DMA2_Stream3->CR |= (1<<26);
	DMA2_Stream3->CR &=~ (1<<27); // Channel 3

	DMA2_Stream3->CR |= (1<<6);
	DMA2_Stream3->CR &=~ (1<<7); // Memory to Peripheral  Direction

	DMA2_Stream3->CR |= DMA_SCR_TCIE;
	DMA2_Stream3->CR |= DMA_SCR_TEIE; // Transfer completed and error interrupts enabled

	DMA2_Stream3->FCR |= DMA2_SFCR_DMDIS; // Disable direct mode

	DMA2_Stream3->FCR |= (1<<0);
	DMA2_Stream3->FCR |= (1<<1); // Set DMA threshold

	NVIC_EnableIRQ(DMA2_Stream3_IRQn); // Enable ISR
}

void dma2_stream2_spi_rx_init(void)
{
	RCC->AHB1ENR |= DMA2_EN;

	DMA2_Stream2->CR = 0;

	while((DMA2_Stream2->CR) & DMA_SCR_EN){}

	DMA2_Stream2->CR |= DMA_SCR_MINC; // DMA Memory increment (won't configure PINC since USART Data register address is static)

	DMA2_Stream2->CR |= (1<<25);
	DMA2_Stream2->CR |= (1<<26);
	DMA2_Stream2->CR &=~ (1<<27); // Channel 3

	DMA2_Stream2->CR &=~ (1<<6);
	DMA2_Stream2->CR &=~ (1<<7); // Peripheral to Memory   Direction

	DMA2_Stream2->CR |= DMA_SCR_TCIE;
	DMA2_Stream2->CR |= DMA_SCR_TEIE; // Transfer completed and error interrupts enabled

	DMA2_Stream2->FCR |= DMA2_SFCR_DMDIS; // Disable direct mode

	DMA2_Stream2->FCR |= (1<<0);
	DMA2_Stream2->FCR |= (1<<1); // Set DMA threshold

	NVIC_EnableIRQ(DMA2_Stream2_IRQn); // Enable ISR
}

void dma2_stream2_spi_receive(uint32_t rx_msg, uint32_t len)
{
	DMA2->LIFCR = LIFCR_CTCIF2 | LIFCR_CTEIF2;

	DMA2_Stream2->PAR = (uint32_t)(&SPI1->DR);

	DMA2_Stream2->M0AR = rx_msg;

	DMA2_Stream2->NDTR = len;

	DMA2_Stream2->CR |= DMA_SCR_EN;
}


void dma2_stream3_spi_transfer(uint32_t tx_msg, uint32_t len)
{
	DMA2->LIFCR = LIFCR_CTCIF3 | LIFCR_CTEIF3;

	DMA2_Stream3->PAR = (uint32_t)(&SPI1->DR);

	DMA2_Stream3->M0AR = tx_msg;

	DMA2_Stream3->NDTR = len;

	DMA2_Stream3->CR |= DMA_SCR_EN;
}















