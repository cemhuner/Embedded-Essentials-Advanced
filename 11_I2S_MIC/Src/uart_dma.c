

#include "uart_dma.h"


#define BAUDRATE    		57600
#define FREQ        		16000000
#define APB1_CLK			FREQ

#define GPIOAEN				(1<<0)
#define UART2EN				(1<<17)

#define CR1_TE				(1<<3) // USART Control Register - Transmitter enable
#define CR1_RE				(1<<2) // USART Control Register - Receiver enable
#define CR1_UE				(1<<13) // USART Control Register - USART enable
#define SR_TXE				(1<<7) // USART Status Register - Transmit data register empty
#define SR_RXNE				(1<<5) // USART Status Register - Receive data register empty

#define CR3_DMAT			(1<<7) // USART Control Register - DMA enable transmitter
#define CR3_DMAR			(1<<6) // USART Control Register - DMA enable receiver

#define SR_TC				(1<<6) // USART Status Register - Transfer Complete
#define CR1_TCIE			(1<<6) // USART Control Register - Transfer Complete Interrupt Enable

#define DMA1_EN				(1<<21)
#define DMA_SCR_EN			(1<<0)  // DMA stream configuration register
#define DMA_SCR_MINC		(1<<10) // DMA Memory Increment
#define DMA_SCR_PINC		(1<<9) // DMA Peripheral Increment
#define DMA_SCR_TCIE		(1<<4) // Transfer complete interrupt
#define DMA_SCR_TEIE		(1<<2) // Transfer error interrupt
#define DMA_SCR_CIRC		(1<<8) // DMA Stream Control Register - Circular Mode

#define HIFCR_CDMEIF5		(1<<8) // Stream 5 High Interrupt Flag Clear Register - Clear Direct Mode Error Interrupt
#define HIFCR_CTEIF5		(1<<9) // Stream 5 High Interrupt Flag Clear Register - Clear Transfer Error Interrupt
#define HIFCR_CTCIF5		(1<<11) // Stream 5 High Interrupt Flag Clear Register - Clear Transfer Complete Interrupt

#define HIFCR_CDMEIF6		(1<<18) // Stream 6 High Interrupt Flag Clear Register - Clear Direct Mode Error Interrupt
#define HIFCR_CTEIF6		(1<<19) // Stream 6 High Interrupt Flag Clear Register - Clear Transfer Error Interrupt
#define HIFCR_CTCIF6		(1<<21) // Stream 6 High Interrupt Flag Clear Register - Clear Transfer Complete Interrupt

#define HIFSR_TCIF5			(1<<11) // Stream 5 High Interrupt Flag Set Register - Transfer Complete Interrupt
#define HIFSR_TCIF6			(1<<21) // Stream 6 High Interrupt Flag Set Register - Transfer Complete Interrupt

static void set_brr(USART_TypeDef *USARTx, uint32_t PeriClk, uint32_t BD);
static uint16_t compute_brr(uint32_t PeriClk, uint32_t BD);
void usart_write(int ch);

char uart_data_buff[5];
uint8_t	g_rx_cmplt;
uint8_t	g_tx_cmplt;

#define AUDIO_BUFFER_SIZE 1024
#define AUDIO_BUFFER_HALF (AUDIO_BUFFER_SIZE / 2)

uint8_t uart_tx_buffer[AUDIO_BUFFER_HALF * 3]; // 3 bytes per 24-bit sample
volatile uint8_t uart_tx_busy = 0;

void uart_rx_tx_init(void)
{
	/*GPIO Configurations*/

	RCC->AHB1ENR |= GPIOAEN; // Enable Clock access to AHB1 Bus

	GPIOA->MODER &=~(1<<4);
	GPIOA->MODER |= (1<<5); // PA2 Alternate function

	GPIOA->MODER &=~(1<<6);
	GPIOA->MODER |= (1<<7); // PA3 Alternate function


	GPIOA->AFR[0] |= (1<<8);
	GPIOA->AFR[0] |= (1<<9);
	GPIOA->AFR[0] |= (1<<10);
	GPIOA->AFR[0] &=~ (1<<11); // PA2 AF[7] (0b0111) UART-TX

	GPIOA->AFR[0] |= (1<<12);
	GPIOA->AFR[0] |= (1<<13);
	GPIOA->AFR[0] |= (1<<14);
	GPIOA->AFR[0] &=~ (1<<15); // PA3 AF[7] (0b0111) UART-RX

	/*USART Configurations*/

	RCC-> APB1ENR |= UART2EN; // Enable Clock access to APB1 Bus

	set_brr(USART2, APB1_CLK, BAUDRATE);  // Set BAUDRATE

	USART2->CR3 = (CR3_DMAT | CR3_DMAR); // Set DMA Transmitter and Receiver

	USART2->CR1 = (CR1_TE | CR1_RE); // USART Recieve and Transmitter enable

	USART2->SR &=~ SR_TC; // Set Transmission is not complete

	USART2->CR1 |= CR1_TCIE;
	USART2->CR1 |= CR1_UE; // enable USART

	NVIC_EnableIRQ(USART2_IRQn); // Enable Interrupt Service Routine for USART2
}

void dma1_init(void)
{
	RCC->AHB1ENR |= DMA1_EN; // Enable DMA

	NVIC_EnableIRQ(DMA1_Stream6_IRQn); // Enable Interrupt Service Routine for DMA
}

void dma1_stream5_uart_rx_config(void)
{

	/*DMA Stream Configurations*/

	DMA1_Stream5->CR &=~ DMA_SCR_EN; // Disable stream

	while ((DMA1_Stream5->CR) & DMA_SCR_EN){} // Checking stream is disabled

	DMA1->HIFCR = HIFCR_CDMEIF5 | HIFCR_CTEIF5 | HIFCR_CTCIF5; // CDMEIF, CTEIF, CTCIF enabled

	DMA1_Stream5->PAR |= (uint32_t)(&(USART2->DR)); // Peripheral Address Register selected

	DMA1_Stream5->M0AR = (uint32_t)(&uart_data_buff[5]); // Memory 0 Address Register selected

	DMA1_Stream5->NDTR = (uint16_t) 5; // Number of Data dedicated

	DMA1_Stream5->CR &=~ (1<<25);
	DMA1_Stream5->CR &=~ (1<<26);
	DMA1_Stream5->CR |= (1<<27); // Channel 4 selected (0b100)

	DMA1_Stream5->CR |= DMA_SCR_MINC; // DMA Memory increment (won't configure PINC since USART Data register address is static)

	DMA1_Stream5->CR |= DMA_SCR_TCIE;
	DMA1_Stream5->CR |= DMA_SCR_TEIE; // Transfer completed and error interrupts enabled

	DMA1_Stream5->CR |= DMA_SCR_CIRC; // DMA in Circular Mode

	DMA1_Stream5->CR &=~ (1<<6);
	DMA1_Stream5->CR &=~ (1<<7); // Peripheral to Memory Direction

	DMA1_Stream5->CR |= DMA_SCR_EN; // DMA stream enabled

	NVIC_EnableIRQ(DMA1_Stream5_IRQn); // Enable Interrupt Service Routine for DMA
}


void dma1_stream6_uart_tx_config(uint32_t send_msg, uint32_t msg_len)
{
	DMA1_Stream6->CR &=~ DMA_SCR_EN; // Disable stream

	while ((DMA1_Stream6->CR) & DMA_SCR_EN){} // Checking stream is disabled

	DMA1->HIFCR = HIFCR_CDMEIF6 | HIFCR_CTEIF6 | HIFCR_CTCIF6; // CDMEIF, CTEIF, CTCIF enabled

	DMA1_Stream6->PAR = (uint32_t)(&(USART2->DR)); // Peripheral Address register selected

	DMA1_Stream6->M0AR = send_msg;

	DMA1_Stream6->NDTR = msg_len;

	DMA1_Stream6->CR &=~ (1<<25);
	DMA1_Stream6->CR &=~ (1<<26);
	DMA1_Stream6->CR |= (1<<27); // Channel 4 selected (0b100)

	DMA1_Stream6->CR |= DMA_SCR_MINC; // DMA Memory increment (won't configure PINC since USART Data register address is static)

	DMA1_Stream6->CR |= (1<<6);
	DMA1_Stream6->CR &=~ (1<<7); // Memory to Peripheral  Direction

	DMA1_Stream6->CR |= DMA_SCR_TCIE;

	DMA1_Stream6->CR |= DMA_SCR_EN; // DMA stream enabled

}

void uart_transmit_audio_data(uint32_t* buffer, uint16_t size)
{
    // Convert 24-bit audio data to bytes for UART transmission
    uint16_t byte_index = 0;

    for(uint16_t i = 0; i < size; i++)
    {
        // Extract 24-bit data and sign extend
        int32_t sample_24bit = (int32_t)(buffer[i] >> 8);
        if(sample_24bit & 0x800000)  // Check sign bit (bit 23)
        {
            sample_24bit |= 0xFF000000;  // Sign extend
        }

        // Pack 24-bit sample into 3 bytes (little-endian format)
        uart_tx_buffer[byte_index++] = (uint8_t)(sample_24bit & 0xFF);         // LSB
        uart_tx_buffer[byte_index++] = (uint8_t)((sample_24bit >> 8) & 0xFF);  // Middle byte
        uart_tx_buffer[byte_index++] = (uint8_t)((sample_24bit >> 16) & 0xFF); // MSB
    }

    // Set transmission busy flag
    uart_tx_busy = 1;

    // Start DMA transmission using your UART driver
    dma1_stream6_uart_tx_config((uint32_t)uart_tx_buffer, byte_index);
}

static void set_brr(USART_TypeDef *USARTx, uint32_t PeriClk, uint32_t BD)
{
	USARTx->BRR = compute_brr(PeriClk, BD);
}


static uint16_t compute_brr(uint32_t PeriClk, uint32_t BD)
{
	return ((PeriClk + (BD/2))/BD);
}

void DMA1_Stream6_IRQHandler(void)
{
	if((DMA1->HISR) & HIFSR_TCIF6)
	{

		DMA1->HIFCR |= HIFCR_CTCIF6;
	}

}

void DMA1_Stream5_IRQHandler(void)
{
	if((DMA1->HISR) & HIFSR_TCIF5)
	{
		g_rx_cmplt = 1;


		DMA1->HIFCR |= HIFCR_CTCIF5;
	}

}

void USART2_IRQHandler(void)
{
	g_tx_cmplt = 1;

	USART2->SR &=~ SR_TC;
}

























