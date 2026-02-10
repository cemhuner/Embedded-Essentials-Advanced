
#include <stdio.h>
#include <string.h>

#include "uart_dma.h"

#include "i2s_mic.h"

#define AUDIO_BUFFER_SIZE 1024
#define AUDIO_BUFFER_HALF (AUDIO_BUFFER_SIZE / 2)

extern uint8_t g_tx_cmplt;
extern uint8_t uart_tx_buffer[AUDIO_BUFFER_HALF * 3];

extern uint32_t audio_buffer[AUDIO_BUFFER_SIZE];

extern volatile uint8_t buffer_half_complete;
extern volatile uint8_t buffer_full_complete;

extern volatile uint8_t uart_tx_busy;


void process_audio_data(uint32_t* buffer, uint16_t size)
{
    // Process received 24-bit audio data from MEMS microphone
    // Data is received as 32-bit words with 24-bit valid data
    // MSB is at bit 31, LSB is at bit 8 (bits 7-0 are padding)

    for(uint16_t i = 0; i < size; i++)
    {
        // Extract 24-bit data from 32-bit word
        // Shift right by 8 to align 24-bit data to LSB
        int32_t sample_24bit = (int32_t)(buffer[i] >> 8);

        // Sign extend from 24-bit to 32-bit
        if(sample_24bit & 0x800000)  // Check sign bit (bit 23)
        {
            sample_24bit |= 0xFF000000;  // Sign extend
        }

        // Here you can process the audio sample
        // For example: apply filters, calculate RMS, detect voice activity, etc.

        // Example: Simple DC removal filter
        static int32_t dc_filter_acc = 0;
        dc_filter_acc += sample_24bit;
        dc_filter_acc -= (dc_filter_acc >> 10);  // Simple DC blocking
        int32_t filtered_sample = sample_24bit - (dc_filter_acc >> 10);

        // Store processed sample back (optional)
        buffer[i] = (uint32_t)filtered_sample;
    }
}

int main(void)
{
    // Configure system clock for 84MHz with proper PLL settings for I2S
    SystemClock_Config();

    // Initialize UART with DMA
    uart_rx_tx_init();
    dma1_init();

    // Configure GPIO pins for I2S
    i2s_init();

    // Configure I2S peripheral for receive
    I2S_Config();

    // Configure DMA for I2S receive
    DMA1_Stream3_SPI2_Config();

    // Clear initial buffers
    for(uint16_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        audio_buffer[i] = 0;
    }

    for(uint16_t i = 0; i < sizeof(uart_tx_buffer); i++)
    {
        uart_tx_buffer[i] = 0;
    }


    while(1)
    {
        // Check for half buffer complete
        if(buffer_half_complete)
        {
            buffer_half_complete = 0;
            // Process first half of buffer
            process_audio_data(&audio_buffer[0], AUDIO_BUFFER_HALF);
            // Transmit processed audio data via UART (non-blocking)
            if(!uart_tx_busy)
            {
                uart_transmit_audio_data(&audio_buffer[0], AUDIO_BUFFER_HALF);
            }
        }

        // Check for full buffer complete
        if(buffer_full_complete)
        {
            buffer_full_complete = 0;
            // Process second half of buffer
            process_audio_data(&audio_buffer[AUDIO_BUFFER_HALF], AUDIO_BUFFER_HALF);
            // Transmit processed audio data via UART (non-blocking)
            if(!uart_tx_busy)
            {
                uart_transmit_audio_data(&audio_buffer[AUDIO_BUFFER_HALF], AUDIO_BUFFER_HALF);
            }
        }

        // Check UART transmission complete
        if(g_tx_cmplt && uart_tx_busy)
        {
            uart_tx_busy = 0;  // Reset transmission busy flag
        }
    }
}
