
// DMA1 Stream6-Channel1 -> I2C1_TX
// DMA1 Stream5-Channel1 -> I2C1_RX




#include "i2c_dma.h"

#define PERIPH_CLK			16 		// 16 MHZ
#define GPIOB_EN			(1<<1)
#define I2C1_EN				(1<<21)
#define CR1_SWRST			(1<<15)
#define CR1_NOSTRETCH		(1<<7)
#define CR1_ENGC			(1<<6)
#define CR2_DMAEN			(1<<11)
#define CR2_LAST			(1<<12)
#define CR1_PE				(1<<0)

#define DMA1_EN				(1<<21)
#define DMA_SCR_EN			(1<<0) 	// DMA1 Stream Configuration Register Stream Enable
#define DMA_SCR_MINC		(1<<10) // DMA1 Stream Configuration Register Memory Increment
#define DMA1_SCR_TCIE		(1<<4) 	// DMA1 Stream Configuration Register Transfer Complete Interrupt Enable
#define HIFCR_CTCIF6		(1<<21) // High Interrupt Flag Clear Register - Transfer Complete Interrupt
#define HIFCR_CTCIF5		(1<<11)
#define HIFSR_TCIF5			(1<<11) // Stream 5 High Interrupt Flag Set Register - Transfer Complete Interrupt
#define HIFSR_TCIF6			(1<<21) // Stream 6 High Interrupt Flag Set Register - Transfer Complete Interrupt


#define SR2_BUSY			(1<<1)
#define CR1_START			(1<<8)
#define SR1_SB				(1<<0)
#define SR1_ADDR			(1<<1)
#define SR1_TXE				(1<<7)
#define CR1_ACK				(1<<10)
#define CR1_STOP			(1<<9)
#define SR1_RXNE			(1<<6)
#define SR1_BTF				(1<<2)
#define CR1_STOP			(1<<9)

uint8_t	gt_rx_cmplt;
uint8_t	gt_tx_cmplt;

void i2c1_gpio_init(void)
{
	/* GPIO CONFIGURATIONS */

	RCC->AHB1ENR |= GPIOB_EN; // Enable access to GPIOB

	GPIOB->MODER &=~ (1<<16);
	GPIOB->MODER |= (1<<17); // PB8 alternate function

	GPIOB->MODER &=~ (1<<18);
	GPIOB->MODER |= (1<<19); // PB9 alternate function

	GPIOB->OTYPER |= (1<<8);
	GPIOB->OTYPER |= (1<<9); // PB8 & PB9 open-drain configuration, I2C works in this configuration

	GPIOB->AFR[1] &=~ (1<<0);
	GPIOB->AFR[1] &=~ (1<<1);
	GPIOB->AFR[1] |=  (1<<2);
	GPIOB->AFR[1] &=~ (1<<3);

	GPIOB->AFR[1] &=~ (1<<4);
	GPIOB->AFR[1] &=~ (1<<5);
	GPIOB->AFR[1] |=  (1<<6);
	GPIOB->AFR[1] &=~ (1<<7); // pb8 & pb9 af04

	/* I2C CONFIGURATIONS */

	RCC->APB1ENR |= I2C1_EN; // Clock access to I2C module

	I2C1->CR1 = CR1_SWRST; // I2C1 Software Reset

	I2C1->CR1 &=~ CR1_SWRST; // Disable I2C1 Software Reset

	I2C1->CR1 &=~ CR1_NOSTRETCH; // No-stretching enable

	I2C1->CR1 &=~ CR1_ENGC; // Disable General Call

	I2C1->CR2 |= CR2_DMAEN; // DMA Enable

	I2C1->CR2 |= CR2_LAST; // Enable LAST bit to generate NACK signal

	I2C1->CR2 |= PERIPH_CLK; // Peripheral bus frequency

	I2C1->CCR = 80;

	I2C1->TRISE = 17;

	I2C1->CR1 |= CR1_PE;

}

void i2c1_dma1_stream6_tx_init(void)
{
	RCC->AHB1ENR |= DMA1_EN; // Clock access to DMA1 module

	DMA1_Stream6->CR = 0;

	while((DMA1_Stream6->CR & DMA_SCR_EN)){}

	DMA1_Stream6->CR |= (1<<25);
	DMA1_Stream6->CR &=~ (1<<26);
	DMA1_Stream6->CR &=~ (1<<27); // Channel 1

	DMA1_Stream6->CR |= DMA_SCR_MINC; // DMA Memory increment (we won't configure PINC since USART Data register address is static)

	DMA1_Stream6->CR |= DMA1_SCR_TCIE; // Transfer completed interrupts enabled

	DMA1_Stream6->CR |= (1<<6);
	DMA1_Stream6->CR &=~ (1<<7); // Memory to Peripheral  Direction

	NVIC_EnableIRQ(DMA1_Stream6_IRQn); // Enable ISR
}

void i2c1_dma1_stream5_rx_init(void)
{
	RCC->AHB1ENR |= DMA1_EN; // Clock access to DMA1 module

	DMA1_Stream5->CR = 0;

	while((DMA1_Stream5->CR & DMA_SCR_EN)){}

	DMA1_Stream5->CR |= (1<<25);
	DMA1_Stream5->CR &=~ (1<<26);
	DMA1_Stream5->CR &=~ (1<<27); // Channel 1

	DMA1_Stream5->CR |= DMA_SCR_MINC; // DMA Memory increment (won't configure PINC since USART Data register address is static)

	DMA1_Stream5->CR |= DMA1_SCR_TCIE; // Enable Transfer Complete Interrupt

	DMA1_Stream5->CR &=~ (1<<6);
	DMA1_Stream5->CR &=~ (1<<7); // Peripheral to Memory   Direction

	NVIC_EnableIRQ(DMA1_Stream5_IRQn); // Enable ISR
}

void i2c1_dma1_stream6_transfer(uint8_t *tx_msg, uint32_t len)
{
	if(NULL != tx_msg)
	{
		DMA1->HIFCR = HIFCR_CTCIF6;

		DMA1_Stream6->PAR = (uint32_t)(&(I2C1->DR));

		DMA1_Stream6->M0AR = (uint32_t)tx_msg;

		DMA1_Stream6->NDTR = len;

		DMA1_Stream6->CR |= DMA_SCR_EN;
	}
	else
	{

	}
}

void i2c1_dma1_stream5_receive(uint8_t *rx_msg, uint32_t len)
{
	if(NULL != rx_msg)
	{
		DMA1->HIFCR = HIFCR_CTCIF5;

		DMA1_Stream5->PAR = (uint32_t)(&I2C1->DR);

		DMA1_Stream5->M0AR = (uint32_t)rx_msg;

		DMA1_Stream5->NDTR = len;

		DMA1_Stream5->CR |= DMA_SCR_EN;
	}
	else
	{

	}
}

void i2c_dma_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_read_buff, uint16_t num_bytes)
{
	while((I2C1->SR2 & SR2_BUSY)){} // Wait until the BUSY flag is set

	I2C1->CR1 |= CR1_START; // Generate start signal

	while(!(I2C1->SR1 & SR1_SB)){} // Wait until start bit flag is set

	I2C1->SR1; // Read SR1

	I2C1->DR = (slave_addr << 1 | 0); // Send slave address

	while(!((I2C1->SR1) & SR1_ADDR)){} // Wait for address flag is set

	I2C1->SR1;

	I2C1->SR2; // To clear address flag we should read SR2 right after the SR1

	while(!((I2C1->SR1) & SR1_TXE)){}

	if(num_bytes >= 2)
	{
		I2C1->CR1 |= CR1_ACK;

		I2C1->DR = reg_addr;
	}
	else
	{
		I2C1->CR1 &=~ CR1_ACK;

		I2C1->DR = reg_addr;
	}

	while(!(I2C1->SR1 & SR1_BTF)){}

	I2C1->CR1 |= CR1_START;

	while(!(I2C1->SR1 & SR1_SB)){}

	I2C1->SR1;

	I2C1->DR = (slave_addr << 1 | 1);

	while(!((I2C1->SR1)& SR1_ADDR)){}

	i2c1_dma1_stream5_receive(p_read_buff, num_bytes);

	I2C1->SR1;

	I2C1->SR2;

}

void i2c_dma_write(uint8_t slave_addr, uint8_t *p_write_buff, uint16_t num_bytes)
{
	while((I2C1->SR2 & SR2_BUSY)){} // Wait until the BUSY flag is set

	I2C1->CR1 |= CR1_START; // Generate start signal

	while(!(I2C1->SR1 & SR1_SB)){} // Wait until start bit flag is set

	I2C1->SR1; // Read SR1

	I2C1->DR = (slave_addr << 1 | 0); // Send slave address

	while(!((I2C1->SR1) & SR1_ADDR)){} // Wait for address flag is set

	i2c1_dma1_stream6_transfer(p_write_buff, num_bytes);

	I2C1->SR1;

	I2C1->SR2; // To clear address flag we should read SR2 right after the SR1
}

void DMA1_Stream6_IRQHandler(void)
{
	if((DMA1->HISR) & HIFSR_TCIF6)
	{
		gt_tx_cmplt = 1;
		I2C1->CR1 |= CR1_STOP; // Generate STOP condition
		DMA1->HIFCR |= HIFCR_CTCIF6;
	}

}

void DMA1_Stream5_IRQHandler(void)
{
	if((DMA1->HISR) & HIFSR_TCIF5)
	{
		gt_rx_cmplt = 1;
		I2C1->CR1 |= CR1_STOP; // Generate STOP condition
		DMA1->HIFCR |= HIFCR_CTCIF5;
	}

}






