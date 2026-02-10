#include "stm32f4xx.h"
#include <stdio.h>
#include "dma.h"
#include "dma_adc.h"
#include <string.h>
#include "mpu9250.h"



float acc_x, acc_y,acc_z;

int main(void)
{
	spi1_dma_init();

	mpu9250_ncs();

	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));

	dma2_stream3_spi_tx_init();

	dma2_stream2_spi_rx_init();

	mpu9250_ncs_reset();

	mpu9250_accel_config(ACC_FULL_SCALE_2G);


	while(1)
	{
		mpu9250_ncs_reset();
		mpu9250_accel_update();

		acc_x = mpu9250_get_x();
		acc_y = mpu9250_get_y();
		acc_z = mpu9250_get_z();

		mpu9250_ncs_set();
	}
}
