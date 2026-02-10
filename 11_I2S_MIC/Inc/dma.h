

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>
#include "stm32f4xx.h"
#define LISR_TCIF0			(1<<5)
#define	LIFCR_CTCIF0		(1<<5)
#define LISR_TEIF0			(1<<3)
#define LIFCR_CTEIF0		(1<<3)
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


void dma2_mem2mem_config(void);
void dma_transfer_init(uint32_t src_buff, uint32_t dest_buff, uint32_t len);


#endif
