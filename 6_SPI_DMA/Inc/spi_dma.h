

#ifndef SPI_DMA_H_
#define SPI_DMA_H_

#include "stm32f4xx.h"
#include <stdint.h>

#define LISR_TCIF3			(1<<27)
#define LISR_TCIF2			(1<<21)

#define LISR_TEIF3		(1<<25)
#define LISR_TEIF2		(1<<19)

#define LIFCR_CTCIF3		(1<<27)
#define LIFCR_CTCIF2		(1<<21)

#define LIFCR_CTEIF3		(1<<25)
#define LIFCR_CTEIF2		(1<<19)


void spi1_dma_init(void);
void dma2_stream3_spi_tx_init(void);
void dma2_stream2_spi_rx_init(void);
void dma2_stream2_spi_receive(uint32_t rx_msg, uint32_t len);
void dma2_stream3_spi_transfer(uint32_t tx_msg, uint32_t len);

#endif
