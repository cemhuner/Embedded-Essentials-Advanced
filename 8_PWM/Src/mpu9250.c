
#include "mpu9250.h"

#define mpu9250_addr				0x68

#define USER_CTRL_I2C_IF_DIS		(1<<4)
#define MAX_TRANSFER_LEN			6
#define READ_FLAG					0x80
#define DMA_SCR_EN					(1<<0)

uint8_t	dummy_buff[MAX_TRANSFER_LEN + 1];
uint8_t accel_buff[MAX_TRANSFER_LEN + 1];

double g_accel_range;
uint8_t i2c_data_buff[2];
extern uint8_t g_tx_cmplt;
extern uint8_t g_rx_cmplt;

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

	i2c_data_buff[0] = MPU9250_ADDR_ACCELCONFIG;
	i2c_data_buff[1] = mode;

	i2c_dma_write(mpu9250_addr, i2c_data_buff, 2);

}

void mpu9250_accel_update(void)
{
	i2c_dma_read(mpu9250_addr, MPU9250_ACCEL_XOUT_H, (uint8_t*) accel_buff, MAX_TRANSFER_LEN);
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







