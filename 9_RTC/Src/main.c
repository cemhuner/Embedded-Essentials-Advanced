#include "rtc.h"
#include "uart.h"
#include <stdio.h>

#define BUFF_LEN		20

uint8_t time_buff[BUFF_LEN] = {0};
uint8_t date_buff[BUFF_LEN] = {0};

int main(void)
{
	uart_tx_rx_init();

	rtc_init();

	while(1)
	{
		display_calendar();
	}
}

void display_calendar(void)
{
	sprintf((char*)time_buff, "%.2d :%.2d :%.2d",rtc_convert_bcd2bin(rtc_time_get_hour()),\
			rtc_convert_bcd2bin(rtc_time_get_min()), rtc_convert_bcd2bin(rtc_time_get_sec()));

	sprintf((char*)date_buff, "%.2d :%.2d :%.2d",rtc_convert_bcd2bin(rtc_date_get_day()),\
				rtc_convert_bcd2bin(rtc_date_get_month()), rtc_convert_bcd2bin(rtc_date_get_years()));

	printf("TIME: %.2d :%.2d :%.2d DATE: %.2d :%.2d :%.2d\n\n\r",rtc_convert_bcd2bin(rtc_time_get_hour()),\
				rtc_convert_bcd2bin(rtc_time_get_min()), rtc_convert_bcd2bin(rtc_time_get_sec()), rtc_convert_bcd2bin(rtc_date_get_day()),\
				rtc_convert_bcd2bin(rtc_date_get_month()), rtc_convert_bcd2bin(rtc_date_get_years()));

}
