
//PLLI2SN = 384, PLLISR = 5, I2SDIV = 12, I2SODD = 1,

#include "I2S.h"

#define AUDIO_BUFFER_SIZE 1024
#define AUDIO_BUFFER_HALF (AUDIO_BUFFER_SIZE / 2)

uint32_t audio_buffer[AUDIO_BUFFER_SIZE];

volatile uint8_t buffer_half_complete = 0;
volatile uint8_t buffer_full_complete = 0;

void i2s_init(void)
{
	RCC->AHB1ENR |= GPIOAEN;

	GPIOA->MODER &=~ (1<<8);
	GPIOA->MODER |=  (1<<9); // PA4 Alternate function

	GPIOA->MODER &=~ (1<<10);
	GPIOA->MODER |=  (1<<11); // PA5 Alternate function

	GPIOA->MODER &=~ (1<<14);
	GPIOA->MODER |=  (1<<15); // PA7 Alternate function

	GPIOA->AFR[0] |= (1<<16);
	GPIOA->AFR[0] &=~ (1<<17);
	GPIOA->AFR[0] |= (1<<18);
	GPIOA->AFR[0] &=~ (1<<19); // PA4 AF05 I2S1_WS

	GPIOA->AFR[0] |= (1<<24);
	GPIOA->AFR[0] &=~ (1<<25);
	GPIOA->AFR[0] |= (1<<26);
	GPIOA->AFR[0] &=~ (1<<27); // PA5 AF05 I2S1_CK

	GPIOA->AFR[0] |= (1<<28);
	GPIOA->AFR[0] &=~ (1<<29);
	GPIOA->AFR[0] |= (1<<30);
	GPIOA->AFR[0] &=~ (1<<31); // PA7 AF05 I2S1_SD

}

void I2S_Config(void)
{
    // Enable SPI2 clock
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    // Configure I2S
    // Reset SPI2
    SPI2->I2SCFGR = 0;
    SPI2->I2SPR = 0;

    // Configure I2S for MEMS microphone:
    // - Master receive mode
    // - I2S Phillips standard
    // - 24-bit data length with 32-bit frame (whole word)
    // - Clock polarity low
    SPI2->I2SCFGR = SPI_I2SCFGR_I2SMOD |        // I2S mode
                    SPI_I2SCFGR_I2SCFG_0 |      // Master receive (01)
                    SPI_I2SCFGR_I2SSTD_0 |      // I2S Phillips standard
                    SPI_I2SCFGR_DATLEN_1 |      // 24-bit data length (10)
                    SPI_I2SCFGR_CHLEN;          // 32-bit channel length (whole word)

    // Configure prescaler with specified values
    // I2SDIV = 12, ODD = 1
    // This gives: Fs = I2S_CLK / (32 * 2 * ((2 * I2SDIV) + ODD))
    // Fs = 76800000 / (64 * ((2 * 12) + 1)) = 76800000 / (64 * 25) = 48000 Hz

    SPI2->I2SPR = (12 << SPI_I2SPR_I2SDIV_Pos) |    // Prescaler = 12
                  (SPI_I2SPR_ODD);                    // Odd prescaler = 1
    SPI2->I2SPR &=~ SPI_I2SPR_MCKOE;     // Disable master clock output
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

void SystemClock_Config(void)
{
    // Enable HSE oscillator
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));

    // Configure PLL for 84MHz system clock
    // HSE = 8MHz, we want 84MHz system clock
    // PLLM = 8, PLLN = 336, PLLP = 4, PLLQ = 7
    // PLL_VCO = (HSE / PLLM) * PLLN = (8 / 8) * 336 = 336MHz
    // System Clock = PLL_VCO / PLLP = 336 / 4 = 84MHz

    RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos) |      // PLLM = 8
                   (336 << RCC_PLLCFGR_PLLN_Pos) |     // PLLN = 336
                   (1 << RCC_PLLCFGR_PLLP_Pos) |       // PLLP = 4 (encoded as 01)
                   (7 << RCC_PLLCFGR_PLLQ_Pos) |       // PLLQ = 7
                   RCC_PLLCFGR_PLLSRC_HSE;              // HSE as PLL source

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    // Configure flash latency for 84MHz
    FLASH->ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;

    // Configure AHB, APB1, APB2 prescalers
    RCC->CFGR = RCC_CFGR_HPRE_DIV1 |     // AHB prescaler = 1
                RCC_CFGR_PPRE1_DIV2 |    // APB1 prescaler = 2 (42MHz)
                RCC_CFGR_PPRE2_DIV1;     // APB2 prescaler = 1 (84MHz)

    // Switch to PLL as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // Configure I2S PLL for 48kHz sampling rate with specified parameters
    // PLLI2SN = 384, PLLI2SR = 5
    // I2S_CLK = ((HSE / PLLM) * PLLI2SN) / PLLI2SR = (8/8 * 384) / 5 = 76.8MHz

    RCC->PLLI2SCFGR = (384 << RCC_PLLI2SCFGR_PLLI2SN_Pos) |  // PLLI2SN = 384
                       (5 << RCC_PLLI2SCFGR_PLLI2SR_Pos);      // PLLI2SR = 5

    // Enable PLLI2S
    RCC->CR |= RCC_CR_PLLI2SON;
    while(!(RCC->CR & RCC_CR_PLLI2SRDY));
}


void DMA1_Stream3_SPI2_Config(void)
{
    // Enable DMA1 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    // Disable DMA stream 3 before configuration (SPI2_RX uses Stream 3)
    DMA1_Stream3->CR &= ~DMA_SxCR_EN;
    while(DMA1_Stream3->CR & DMA_SxCR_EN);

    // Clear all flags
    DMA1->LIFCR = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3;

    // Configure DMA Stream 3 for SPI2_RX
    DMA1_Stream3->PAR = (uint32_t)&(SPI2->DR);              // Peripheral address
    DMA1_Stream3->M0AR = (uint32_t)audio_buffer;            // Memory address
    DMA1_Stream3->NDTR = AUDIO_BUFFER_SIZE;                 // Number of data items

    // Configure DMA:
    // - Channel 0 (SPI2_RX)
    // - Peripheral to memory
    // - Circular mode
    // - Memory increment
    // - 32-bit data size (for 24-bit data with 32-bit frame)
    // - High priority
    // - Transfer complete and half transfer interrupts
    DMA1_Stream3->CR = (0 << DMA_SxCR_CHSEL_Pos) |         // Channel 0
                       (0 << DMA_SxCR_DIR_Pos) |            // Peripheral to memory
                       DMA_SxCR_CIRC |                      // Circular mode
                       DMA_SxCR_MINC |                      // Memory increment
                       DMA_SxCR_MSIZE_1 |                   // Memory 32-bit
                       DMA_SxCR_PSIZE_1 |                   // Peripheral 32-bit
                       DMA_SxCR_PL_1 |                      // High priority
                       DMA_SxCR_TCIE |                      // Transfer complete interrupt
                       DMA_SxCR_HTIE;                       // Half transfer interrupt

    // Enable DMA1 Stream 3 interrupt
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    // Enable SPI2 DMA receive
    SPI2->CR2 |= SPI_CR2_RXDMAEN;

    DMA1_Stream3->CR |= DMA_SxCR_EN;
}

void DMA1_Stream3_IRQHandler(void)
{
    // Check for half transfer complete
    if(DMA1->LISR & DMA_LISR_HTIF3)
    {
        DMA1->LIFCR = DMA_LIFCR_CHTIF3;  // Clear flag
        buffer_half_complete = 1;
    }

    // Check for transfer complete
    if(DMA1->LISR & DMA_LISR_TCIF3)
    {
        DMA1->LIFCR = DMA_LIFCR_CTCIF3;  // Clear flag
        buffer_full_complete = 1;
    }

    // Check for errors
    if(DMA1->LISR & (DMA_LISR_TEIF3 | DMA_LISR_DMEIF3 | DMA_LISR_FEIF3))
    {
        DMA1->LIFCR = DMA_LIFCR_CTEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3;
        // Handle error - could restart DMA or set error flag
    }
}
