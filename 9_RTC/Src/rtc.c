
#include "rtc.h"

#define PWR_EN			(1<<28)
#define CR_DBP			(1<<8)
#define CSR_LSI_ON		(1<<0)
#define CSR_LSI_RDY		(1<<1)
#define BDCR_BDRST		(1<<16)
#define BDCR_RTC_EN		(1<<15)
#define ISR_INITF		(1<<6)

#define RTC_WRITE_PROTECTION_ENABLE_1 ((uint8_t) 0xCA)
#define RTC_WRITE_PROTECTION_ENABLE_2 ((uint8_t) 0x53)
#define RTC_INIT_MASK				  0xFFFFFFFF

#define WEEKDAY_FRI		0x05
#define MONTH_DEC		0x12
#define TIME_FORMAT_PM	(1<<22)
#define CR_FMT			(1<<6)
#define ISR_RSF			(1<<5)

#define RTC_ASYNCH_PREDIV		((uint32_t)0x7F)
#define RTC_SYNCH_PREDIV		((uint32_t)0x00F9)

static void rtc_time_config(uint32_t Format12_24, uint32_t Hours, uint32_t Minutes, uint32_t Seconds);
static void rtc_date_config(uint32_t WeekDay, uint32_t Day, uint32_t Month, uint32_t Year);
static void rtc_set_asynch_prescaler(uint32_t AsynchPrescaler);
static void rtc_set_synch_prescaler(uint32_t SynchPrescaler);
static uint8_t rtc_init_seq();
static uint8_t exit_init_seq(void);

void rtc_init(void)
{
	 RCC->APB1ENR |= PWR_EN;

	 PWR->CR |= CR_DBP;

	 RCC->CSR |= CSR_LSI_ON;

	 while((RCC->CSR & CSR_LSI_RDY) != CSR_LSI_RDY){}

	 RCC->BDCR |= BDCR_BDRST;

	 RCC->BDCR &=~ BDCR_BDRST;

	 RCC->BDCR &=~ (1<<8);
	 RCC->BDCR |=  (1<<9);

	 RCC->BDCR |= BDCR_RTC_EN;

	 RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_1;
	 RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_2;

	 if(!rtc_init_seq())
	 {

	 }

	 rtc_date_config(WEEKDAY_FRI, 0x29, MONTH_DEC, 0x16);

	 rtc_time_config(TIME_FORMAT_PM, 0x00, 0x00, 0x00);

	 rtc_set_asynch_prescaler(RTC_ASYNCH_PREDIV);

	 rtc_set_synch_prescaler(RTC_SYNCH_PREDIV);

	 exit_init_seq();

	 RTC->WPR = 0xFF;
}

uint8_t rtc_convert_bin2bcd(uint8_t value)
{
	return (uint8_t)((((value) / 10) << 4) | ((value) % 10));
}

uint8_t rtc_convert_bcd2bin(uint8_t value)
{
	return (uint8_t)(((uint8_t)((value) & (uint8_t)0xF0) >> (uint8_t)0x04) * 10 + ((value) & (uint8_t)0x0F));
}

uint32_t rtc_date_get_day(void)
{
	return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_DT | RTC_DR_DU))) >> RTC_DR_DU_Pos);
}

uint32_t rtc_date_get_years(void)
{
	return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_YT | RTC_DR_YU))) >> RTC_DR_YU_Pos);
}

uint32_t rtc_date_get_month(void)
{
	return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_MT | RTC_DR_MU))) >> RTC_DR_MU_Pos);
}

uint32_t rtc_time_get_sec(void)
{
	return (uint32_t)((READ_BIT(RTC->TR, (RTC_TR_ST | RTC_TR_SU))) >> RTC_TR_SU_Pos);
}

uint32_t rtc_time_get_min(void)
{
	return (uint32_t)((READ_BIT(RTC->TR, (RTC_TR_MNT | RTC_TR_MNU))) >> RTC_TR_MNU_Pos);
}

uint32_t rtc_time_get_hour(void)
{
	return (uint32_t)((READ_BIT(RTC->TR, (RTC_TR_HT | RTC_TR_HU))) >> RTC_TR_HU_Pos);
}

static void rtc_set_asynch_prescaler(uint32_t AsynchPrescaler)
{
	MODIFY_REG(RTC->PRER, RTC_PRER_PREDIV_A, AsynchPrescaler << RTC_PRER_PREDIV_A_Pos);
}

static void rtc_set_synch_prescaler(uint32_t SynchPrescaler)
{
	MODIFY_REG(RTC->PRER, RTC_PRER_PREDIV_S, SynchPrescaler);
}

static void rtc_date_config(uint32_t WeekDay, uint32_t Day, uint32_t Month, uint32_t Year)
{
	register uint32_t temp = 0;

	temp = (WeekDay << RTC_DR_WDU_Pos) | \
			(((Year & 0xF0) << (RTC_DR_YT_Pos - 4)) | ((Year & 0x0F) << RTC_DR_YU_Pos)) | \
			(((Month & 0xF0) << (RTC_DR_MT_Pos - 4)) | ((Month & 0x0F) << RTC_DR_MU_Pos)) | \
			(((Day & 0xF0) << (RTC_DR_DT_Pos - 4)) | ((Day & 0x0F) << RTC_DR_DU_Pos));
	MODIFY_REG(RTC->DR, (RTC_DR_WDU | RTC_DR_MT | RTC_DR_MU | RTC_DR_DT | RTC_DR_DU | RTC_DR_YT | RTC_DR_YU), temp);
}

static void rtc_time_config(uint32_t Format12_24, uint32_t Hours, uint32_t Minutes, uint32_t Seconds)
{
	register uint32_t temp = 0;
	temp = Format12_24 | \
			(((Hours & 0xF0) << (RTC_TR_HT_Pos - 4)) | ((Hours & 0x0F) << RTC_TR_HU_Pos)) | \
			(((Minutes & 0xF0) << (RTC_TR_MNT_Pos - 4)) | ((Minutes & 0x0F) << RTC_TR_MNU_Pos)) | \
			(((Seconds & 0xF0) << (RTC_TR_ST_Pos - 4)) | ((Seconds & 0x0F) << RTC_TR_SU_Pos));
	MODIFY_REG(RTC->TR, (RTC_TR_PM | RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU | RTC_TR_ST | RTC_TR_SU), temp);
}

void _rtc_enable_init_mode(void)
{
	RTC->ISR = RTC_INIT_MASK;
}

void _rtc_disable_init_mode(void)
{
	RTC->ISR = ~RTC_INIT_MASK;
}

uint8_t _rtc_isActiveflag_init(void)
{
	return ((RTC->ISR & ISR_INITF) == ISR_INITF);

}
uint8_t _rtc_isActiveflag_rs(void)
{
	return ((RTC->ISR & ISR_RSF) == ISR_RSF);
}

uint8_t rtc_init_seq(void)
{
	_rtc_enable_init_mode();

	while (_rtc_isActiveflag_init() != 1){}

	return 1;
}
static uint8_t wait_synch(void)
{
	RTC->ISR &=~ ISR_RSF;

	while(_rtc_isActiveflag_rs() != 1){}

	return 1;
}

static uint8_t exit_init_seq(void)
{
	_rtc_disable_init_mode();

	return wait_synch();
}



