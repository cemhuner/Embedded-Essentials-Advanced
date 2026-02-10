
#include <stdio.h>
#include <string.h>
#include "mpu9250.h"


float acc_x, acc_y,acc_z;

int main(void)
{
	i2c1_gpio_init();

	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));

	i2c1_dma1_stream6_tx_init();

	i2c1_dma1_stream5_rx_init();

	mpu9250_accel_config(ACC_FULL_SCALE_2G);


	while(1)
	{
		mpu9250_accel_update();

		acc_x = mpu9250_get_x();
		acc_y = mpu9250_get_y();
		acc_z = mpu9250_get_z();

	}
}
