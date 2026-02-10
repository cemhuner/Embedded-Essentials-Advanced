
/* PB8  AF04 -> I2C1_SCL (D15)
 * PB9  AF04 -> I2C1_SDA (D14)
*/


#include "stm32f4xx.h"


#define GPIOBEN					(1<<1)
#define I2C1EN					(1<<21)
#define I2C_100KHZ				80
#define SD_MAX_RISE_TIME 		17
#define CR1_PE					(1<<0)
#define SR2_BUSY				(1<<1)
#define CR1_START				(1<<8)
#define SR1_SB					(1<<0)
#define SR1_ADDR				(1<<1)
#define SR1_TXE					(1<<7)
#define CR1_ACK					(1<<10)
#define CR1_STOP				(1<<9)
#define SR1_RXNE				(1<<6)
#define SR1_BTF					(1<<2)




void I2C1_init(void)
{
	RCC->AHB1ENR |= GPIOBEN; // Enable access to GPIOB

	GPIOB->MODER &=~ (1<<16);
	GPIOB->MODER |= (1<<17); // PB8 alternate function

	GPIOB->MODER &=~ (1<<18);
	GPIOB->MODER |= (1<<19); // PB9 alternate function

	GPIOB->OTYPER |= (1<<8);
	GPIOB->OTYPER |= (1<<9); // PB8 & PB9 open-drain configuration, I2C works in this configuration

	GPIOB->PUPDR |= (1<<16);
	GPIOB->PUPDR &=~ (1<<17); // PB8 pull-down

	GPIOB->PUPDR |= (1<<18);
	GPIOB->PUPDR &=~ (1<<19); // PB9 pull-down

	GPIOB->AFR[1] &=~ (1<<0);
	GPIOB->AFR[1] &=~ (1<<1);
	GPIOB->AFR[1] |= (1<<2);
	GPIOB->AFR[1] &=~ (1<<3);

	GPIOB->AFR[1] &=~ (1<<4);
	GPIOB->AFR[1] &=~ (1<<5);
	GPIOB->AFR[1] |= (1<<6);
	GPIOB->AFR[1] &=~ (1<<7); // pb8 & pb9 af04

	RCC->APB1ENR |= I2C1EN; // Enable clock access to I2C1

	I2C1->CR1 |= (1<<15); // Software reset
	I2C1->CR1 &=~ (1<<15); // disable software reset

	/* --SETTING UP CLOCK--
	 * For 100KHz data transfer speed, in 16MHz PCLK (Peripheral Clock Frequency) FREQR must be 0b01000 = 62.5 ns
	 * So, CCR must be 80 in decimal since 80x62.5 ns = 5000 ns
	 * Actually 100KHz equals 10K ns but we aim to reach 5000 ns since we are using 2 duty cycle ratio
	 * TRISE: Max SCL rise time is 1000 ns, if FREQR 0b01000 Tpclk equals 62.5 ns and TRISE bits must be 17 in decimal [(1000/62.5) = 16+1]
	 * !! TRISE must be configured only when CR_PE = 0
	 */

	I2C1->CR2 = (1<<4); // setting clock frequency, 16 mhz in binary (0b01000)
	I2C1->CCR = I2C_100KHZ; // data transfer speed
	I2C1->TRISE = SD_MAX_RISE_TIME;
	I2C1->CR1 |= CR1_PE;

}

void I2C1_byteRead(char saddr, char maddr, char* data)
{

	/* --I2C DEVICE ADDRESSING ROUTINE--
	 * _____________________________________________________________________________ /
	 * FOR SINGLE-BYTE READ:														|/
	 * 																				|/
	 * 1) GENERATE START SIGNAL (OCCURS WHEN BOTH SDA AND SCL LINES DROP LOW)		|/
	 * 2) SLAVE ADDRESS + WRITE (UNIQUE DEVICE ID INTO DATA REGISTER)				|/
	 * 3) SLAVE-ACK 																|/
	 * 4) SEND SLAVE MEMORY ADDRESS (ADDRESS OF THE RELATED REGISTER E.G. DEV_ID)	|/
	 * 5) SLAVE-ACK																	|/
	 * 6) GENERATE RESTART															|/
	 * 7) SLAVE ADDRESS + READ														|/
	 * 8) SLAVE-ACK																	|/
	 * 9) SLAVE SENDS DATA															|/
	 * 10) DISABLE ACK																|/
	 * 11) STOP																		|/
	 *  */

	volatile int tmp; // temporary variable to clear address flag

	while (I2C1->SR2 & (SR2_BUSY)){} // wait until I2C1 is not busy

	I2C1->CR1 |= CR1_START; // START signal

	while (!(I2C1->SR1 & (SR1_SB))){} // wait until start flag is set

	I2C1->DR = (saddr << 1); // transmit slave address (unique device address in I2C line), slave addresses are 7 bits
							 // In I2C and LSB bit is used to R/W operations so bits are shifted one bit left

	while (!(I2C1->SR1 & (SR1_ADDR))){} // wait until address flag is set

	tmp = I2C1->SR2; // clear address flag (necessary)

	I2C1->DR = maddr; // send slave memory address

	while (!(I2C1->SR1 & SR1_TXE)){} // wait until transmitter empty

	I2C1->CR1 |= CR1_START; // RESTART

	while (!(I2C1->SR1 & (SR1_SB))){} // wait until start flag is set

	I2C1->DR = saddr << 1 | 1; // LSB bit 1 = read

	while (!(I2C1->SR1 & (SR1_ADDR))){} // wait until address flag is set

	I2C1->CR1 &=~ CR1_ACK; // Disable acknowledge

	tmp = I2C1->SR2; // clear address flag

	I2C1->CR1 |= CR1_STOP; // STOP after data received

	while (!(I2C1->SR1 & SR1_RXNE)){} // wait until RXNE flag is set

	*data++ = I2C1->DR;

}

void I2C1_burstRead(char saddr, char maddr, int n, char* data)
{
	/*		______________________________________________________________________________
		 * _____________________________________________________________________________ /|
		 * FOR MULTIPLE-BYTE READ:														|/|
		 * 																				|/|
		 * 1) GENERATE START SIGNAL (OCCURS WHEN BOTH SDA AND SCL LINES DROP LOW)		|/|
		 * 2) SLAVE ADDRESS + WRITE (UNIQUE DEVICE ID INTO DATA REGISTER)				|/|
		 * 3) SLAVE-ACK 																|/|
		 * 4) SEND SLAVE MEMORY ADDRESS (ADDRESS OF THE RELATED REGISTER E.G. DEV_ID)	|/|
		 * 5) SLAVE-ACK																	|/|
		 * 6) GENERATE RESTART															|/|
		 * 7) SLAVE ADDRESS + READ														|/|
		 * 8) SLAVE-ACK			] ITERATES BETWEEN THIS 2 STEPS UNTIL DATA IS ACQUIRED	|/|
		 * 9) SLAVE SENDS DATA	]														|/|
		 * 10) STOP																		|/
		*/

	volatile int tmp;

	while (I2C1->SR2 & (SR2_BUSY)){} // wait until I2C1 is not busy

	I2C1->CR1 |= CR1_START; // START signal

	while (!(I2C1->SR1 & (SR1_SB))){} // wait until start flag is set

	I2C1->DR = (saddr << 1); // transmit slave address

	while (!(I2C1->SR1 & (SR1_ADDR))){} // wait until address flag is set

	tmp = I2C1->SR2; // clear address flag

	while (!(I2C1->SR1 & SR1_TXE)){} // wait until transmitter empty

	I2C1->DR = maddr; // send slave memory address

	while (!(I2C1->SR1 & SR1_TXE)){} // wait until transmitter empty

	I2C1->CR1 |= CR1_START; // RESTART

	while (!(I2C1->SR1 & (SR1_SB))){} // wait until start flag is set

	I2C1->DR = saddr << 1 | 1;

	while (!(I2C1->SR1 & (SR1_ADDR))){} // wait until address flag is set

	tmp = I2C1->SR2; // clear address flag

	I2C1->CR1 |= CR1_ACK;

	while (n > 0)
	{
		if (n==1)
		{
			I2C1->CR1 &=~ CR1_ACK; // if one byte, disable acknowledge

			I2C1->CR1 |= CR1_STOP; // STOP after data received

			while (!(I2C1->SR1 & SR1_RXNE)){} // wait until RXNE flag is set

			*data++ = I2C1->DR;
			break;
		}
		else
		{
			while (!(I2C1->SR1 & SR1_RXNE)){} // wait until RXNE flag is set
			*data++ = I2C1->DR;
			n--;
		}
	}



}

void I2C1_burstWrite(char saddr, char maddr, int n, char* data)
{
	volatile int tmp;

	while (I2C1->SR2 & (SR2_BUSY)){} // wait until I2C1 is not busy

	I2C1->CR1 |= CR1_START; // START signal

	while (!(I2C1->SR1 & (SR1_SB))){} // wait until start flag is set

	I2C1->DR = (saddr << 1); // transmit slave address

	while (!(I2C1->SR1 & (SR1_ADDR))){} // wait until address flag is set

	tmp = I2C1->SR2; // clear address flag

	while (!(I2C1->SR1 & SR1_TXE)){} // wait until transmitter empty

	I2C1->DR = maddr; // send slave memory address

	for (int i = 0; i<n ; i++)
	{
		while (!(I2C1->SR1 & SR1_TXE)){} // wait until transmitter empty

		I2C1->DR = *data++;

	}

	while (!(I2C1->SR1 & (SR1_BTF))){}

	I2C1->CR1 |= CR1_STOP;

}


