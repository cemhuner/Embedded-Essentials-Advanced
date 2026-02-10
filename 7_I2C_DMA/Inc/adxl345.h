/*
 * adxl345.h
 *
 *  Created on: Aug 9, 2025
 *      Author: orhan
 */

#ifndef ADXL345_H_
#define ADXL345_H_


#include "i2c_dma.h"

#define DEVID_R				0x00
#define DEVICE_ADDR			0x53
#define DATA_FORMAT			0x31
#define DATA_START_ADDR		0x32
#define POWER_CTL			0x2D
#define FOUR_G				0x01
#define RESET				0x00
#define SET_MEASURE_B		0x08




void adxl345_init(void);
void adxl345_read_values(uint8_t reg);





#endif /* ADXL345_H_ */
