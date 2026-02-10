
#ifndef MPU9250_H_
#define MPU9250_H_

#include <stdint.h>
#include "spi_dma.h"

#define ACC_FULL_SCALE_2G		0x00
#define ACC_FULL_SCALE_4G		0x08
#define ACC_FULL_SCALE_8G		0x10
#define ACC_FULL_SCALE_16G		0x18

#define MPU9250_ADDR_ACCELCONFIG		0x1C
#define MPU9250_ADDR_USER_CTRL			0x6A
#define MPU9250_ACCEL_XOUT_H			0x3B

float mpu9250_get_z(void);
float mpu9250_get_y(void);
float mpu9250_get_x(void);
float mpu9250_accel_get(uint8_t high_index, uint8_t low_index);
void mpu9250_accel_update(void);
void mpu9250_accel_config(uint8_t mode);
void mpu9250_ncs_reset(void);
void mpu9250_ncs_set(void);
void mpu9250_ncs(void);

#endif
