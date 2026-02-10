
#include "mpu9250.h"

#define USER_CTRL_I2C_IF_DIS		(1<<4)
#define MAX_TRANSFER_LEN			6
#define READ_FLAG					0x80
#define DMA_SCR_EN					(1<<0)

uint8_t	dummy_buff[MAX_TRANSFER_LEN + 1];
uint8_t accel_buff[MAX_TRANSFER_LEN + 1];

double g_accel_range;
uint8_t spi_data_buff[2];
extern uint8_t g_tx_cmplt;
extern uint8_t g_rx_cmplt;


void mpu9250_ncs(void)
{
	RCC->AHB1ENR |= (1<<0);

	GPIOA->MODER |= (1<<0);
	GPIOA->MODER &=~ (1<<1);
}

void mpu9250_ncs_set(void)
{
	GPIOA->ODR |= (1<<0);
}

void mpu9250_ncs_reset(void)
{
	GPIOA->ODR &=~ (1<<0);
}


void mpu9250_accel_config(uint8_t mode)
{
	switch(mode)
	{
	case ACC_FULL_SCALE_2G:
		g_accel_range = 2.0;
		break;

	case ACC_FULL_SCALE_4G:
		g_accel_range = 4.0;
		break;

	case ACC_FULL_SCALE_8G:
		g_accel_range = 8.0;
		break;

	case ACC_FULL_SCALE_16G:
		g_accel_range = 16.0;
		break;
	default:
		break;
	}

	/*Set to SPI mode only*/

	spi_data_buff[0] = MPU9250_ADDR_USER_CTRL;
	spi_data_buff[1] = USER_CTRL_I2C_IF_DIS; // Disable I2C, only SPI

	dma2_stream3_spi_transfer((uint32_t) spi_data_buff, (uint32_t) 2);

	while(!g_tx_cmplt){}

	g_tx_cmplt = 0;

	spi_data_buff[0] = MPU9250_ADDR_ACCELCONFIG;
	spi_data_buff[1] = mode;

	dma2_stream3_spi_transfer((uint32_t) spi_data_buff, (uint32_t) 2);

	while(!g_tx_cmplt){}

	g_tx_cmplt = 0;

}

void mpu9250_accel_update(void)
{
	dummy_buff[0] = MPU9250_ACCEL_XOUT_H | READ_FLAG;

	dma2_stream2_spi_receive((uint32_t)accel_buff, (MAX_TRANSFER_LEN + 1));
	dma2_stream3_spi_transfer((uint32_t) dummy_buff, (uint32_t) (MAX_TRANSFER_LEN + 1));

	while(!g_rx_cmplt){}
	g_rx_cmplt = 0;

}

float mpu9250_accel_get(uint8_t high_index, uint8_t low_index)
{
	int16_t result = accel_buff[high_index] << 8 | accel_buff[low_index];

	if(result)
	{
		return ((float) - result) * g_accel_range / (float) 0x8000;
	}
	else
	{
		return 0.0;
	}

}

float mpu9250_get_x(void)
{
	return mpu9250_accel_get(1,2);
}

float mpu9250_get_y(void)
{
	return mpu9250_accel_get(3,4);
}

float mpu9250_get_z(void)
{
	return mpu9250_accel_get(5,6);
}

void DMA2_Stream3_IRQHandler(void)
{
	if((DMA2->LISR) & LISR_TCIF3)
		{
			g_tx_cmplt = 1;

			DMA2_Stream3->CR &=~ DMA_SCR_EN;

			DMA2->LIFCR |= LIFCR_CTCIF3; // Clear the flag
		}
	else if((DMA2->LISR) & LISR_TEIF3)
		{
			DMA2->LIFCR |= LIFCR_CTEIF3; // Clear the flag
		}
}

void DMA2_Stream2_IRQHandler(void)
{
	if((DMA2->LISR) & LISR_TCIF2)
		{
			g_rx_cmplt = 1;

			DMA2_Stream2->CR &=~ DMA_SCR_EN;

			DMA2->LIFCR |= LIFCR_CTCIF2; // Clear the flag
		}
	else if((DMA2->LISR) & LISR_TEIF2)
		{
			DMA2->LIFCR |= LIFCR_CTEIF2; // Clear the flag
		}

}





