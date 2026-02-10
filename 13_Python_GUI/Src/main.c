#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "adxl345.h"



int16_t x,y,z;
float xg, yg, zg;
extern uint8_t data_rec[6];
int values[3];

int main(void)
{

	adxl345_init();

	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));

	uart_tx_rx_init();

	while(1)
	{
		adxl345_read_values(DATA_START_ADDR);

		x = ((data_rec[1] << 8) | data_rec[0]);
		y = ((data_rec[3] << 8) | data_rec[2]);
		z = ((data_rec[5] << 8) | data_rec[4]);

		values[0] = x; values[1] = y; values[2] = z;

		MSG_SendSensorData(values, 3);


		//printf("%d\n\r", (int)xg);

	}

}






