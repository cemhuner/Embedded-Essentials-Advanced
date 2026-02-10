

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>
#include "stm32f4xx.h"
#define LISR_TCIF0			(1<<5)
#define	LIFCR_CTCIF0		(1<<5)
#define LISR_TEIF0			(1<<3)
#define LIFCR_CTEIF0		(1<<3)

void dma2_mem2mem_config(void);
void dma_transfer_init(uint32_t src_buff, uint32_t dest_buff, uint32_t len);


#endif
