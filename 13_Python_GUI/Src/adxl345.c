
#include "adxl345.h"

char data;
uint8_t data_rec[6];


void adxl345_read_address(uint8_t reg)
{

	I2C1_byteRead(DEVICE_ADDR, reg, &data);

}

void adxl345_write(uint8_t reg, char value)
{
	char data[1];
	data[0] = value;

	I2C1_burstWrite(DEVICE_ADDR, reg, 1 , data);
}

void adxl345_read_values(uint8_t reg)
{
	I2C1_burstRead(DEVICE_ADDR, reg, 6, (char*) data_rec);
}

void adxl345_init(void)
{
	I2C1_init();
	adxl345_read_address(DEVID_R); // Read DEVID, should return 0xE5;
	adxl345_write(DATA_FORMAT, FOUR_G); // Set data format range to -+4g
	adxl345_write(POWER_CTL, RESET); // Reset all bits
	adxl345_write(POWER_CTL, SET_MEASURE_B); // Configure power control measure bit


}
