
#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include "stm32f4xx.h"

void rtc_init(void);
uint8_t rtc_convert_bin2bcd(uint8_t value);
uint8_t rtc_convert_bcd2bin(uint8_t value);
uint32_t rtc_date_get_day(void);
uint32_t rtc_date_get_years(void);
uint32_t rtc_date_get_month(void);
uint32_t rtc_time_get_sec(void);
uint32_t rtc_time_get_min(void);
uint32_t rtc_time_get_hour(void);

#endif
