#include "spi_serve.h"

void SPI_TransmitReceive_DMA(dma_channel_type* SPI_DMA_Transmit_Channel,\
														 dma_channel_type* SPI_DMA_Receive_Channel,\
														 uint32_t          Peripheral_Address,\
														 uint32_t          Transmit_Buffer_Address,\
														 uint32_t          Reseive_Buffer_Address,\
														 uint32_t          Size)
{
	wk_dma_channel_config(SPI_DMA_Transmit_Channel, 
                        Peripheral_Address, 
                        Transmit_Buffer_Address, 
                        Size);
	wk_dma_channel_config(SPI_DMA_Receive_Channel, 
                        Peripheral_Address, 
                        Reseive_Buffer_Address, 
                        Size);
	
	dma_channel_enable(SPI_DMA_Transmit_Channel, TRUE);
  dma_channel_enable(SPI_DMA_Receive_Channel, TRUE);
}

