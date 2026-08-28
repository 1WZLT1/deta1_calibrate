/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f435_437_wk_config.h"
#include "wk_exint.h"
#include "wk_spi.h"
#include "wk_tmr.h"
#include "wk_usart.h"
#include "wk_dma.h"
#include "wk_gpio.h"
#include "wk_system.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "imu_task.h"
#include "rtthread.h"
#include "lsm6dsr.h"
#include "xv7001.h"
#include "scha16t.h"
#include "icm42688.h"
#include "FDILinkManager.h"

#include "devicetree_generated.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */
#if (DT_LSM6DS3TR_ENABLED == 1) && (DT_IIM42652_ENABLED == 1)
	#error "LSM6DS3TR and IIM42652 cannot be enabled at the same time"
#endif

#if (DT_LSM6DSRTR_ENABLED == 1) && (DT_IIM42652_ENABLED == 1)
#error "LSM6DS3TR and IIM42652 cannot be enabled at the same time"
#endif

void SystemCoreClockUpdate(void)
{
	
}

void thread_init()
{
	__disable_irq();
	imu_task_init();
	Sensor_Init();
	FDILinkManager_Init();
	FDILink_Receive_Init();
	__enable_irq();
}

void FDI_Flash_EOPB0_Set(void)
{
	uint16_t cur = USD->eopb0 & 0x0007;
	if(cur != 0x04)
	{
		flash_unlock();
		flash_user_system_data_erase();
		flash_eopb0_config(FLASH_EOPB0_SRAM_256K);
		flash_lock();
		NVIC_SystemReset();
	}
}

uint8_t all_init = 0;

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */
/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */
	
  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /**
   * users need add interrupt handler code into the below function in the at32f435_437_int.c file.
   *  --void SystTick_IRQHandler(void)
   */
  systick_interrupt_config(1000);

  /* nvic config. */
  wk_nvic_config();

  /* init gpio function. */
  wk_gpio_config();

  /* init dma1 channel1 */
  wk_dma1_channel1_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL1, 
                        (uint32_t)&USART1->dt, 
                        DMA1_CHANNEL1_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL1_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* init dma1 channel2 */
  wk_dma1_channel2_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL2, 
                        (uint32_t)&USART1->dt, 
                        DMA1_CHANNEL2_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL2_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL2, TRUE);

  /* init dma1 channel5 */
  wk_dma1_channel5_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL5, 
                        (uint32_t)&SPI1->dt, 
                        DMA1_CHANNEL5_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL5_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL5, TRUE);

  /* init dma1 channel6 */
  wk_dma1_channel6_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL6, 
                        (uint32_t)&SPI1->dt, 
                        DMA1_CHANNEL6_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL6_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL6, TRUE);

  /* init dma2 channel4 */
  wk_dma2_channel4_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA2_CHANNEL4, 
                        (uint32_t)&SPI2->dt, 
                        DMA2_CHANNEL4_MEMORY_BASE_ADDR, 
                        DMA2_CHANNEL4_BUFFER_SIZE);
  dma_channel_enable(DMA2_CHANNEL4, TRUE);

  /* init dma2 channel5 */
  wk_dma2_channel5_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA2_CHANNEL5, 
                        (uint32_t)&SPI2->dt, 
                        DMA2_CHANNEL5_MEMORY_BASE_ADDR, 
                        DMA2_CHANNEL5_BUFFER_SIZE);
  dma_channel_enable(DMA2_CHANNEL5, TRUE);

  /* init usart1 function. */
  wk_usart1_init();

  /* init spi1 function. */
  wk_spi1_init();

  /* init spi2 function. */
  wk_spi2_init();

  /* init exint function. */
  wk_exint_config();

  /* init tmr1 function. */
  wk_tmr1_init();

  /* init tmr2 function. */
  wk_tmr2_init();

  /* init tmr4 function. */
  wk_tmr4_init();

  /* init tmr5 function. */
  wk_tmr5_init();

  /* add user code begin 2 */
	
	flash_nzw_boost_enable(TRUE);
	flash_continue_read_enable(TRUE);
	
	dma_channel_enable(DMA1_CHANNEL1, FALSE);
	dma_channel_enable(DMA1_CHANNEL2, FALSE);
	
	dma_channel_enable(DMA1_CHANNEL5, FALSE);
	dma_channel_enable(DMA1_CHANNEL6, FALSE);
	
	dma_channel_enable(DMA2_CHANNEL4, FALSE);
	dma_channel_enable(DMA2_CHANNEL5, FALSE);
	
	gpio_bits_write(GPIOA,GPIO_PINS_15,TRUE);
	
	#if (DT_LSM6DS3TR_ENABLED == 1 || DT_LSM6DSRTR_ENABLED == 1)
	LSM6DSR_Init();
	#endif
	#if(DT_IIM42652_ENABLED == 1)
	iotex_icm42605_init();
	#endif
	#if(DT_SCHA1633_ENABLED == 1)
	SCHA16T_Init();
	#endif
	#if(DT_XV7011_ENABLED == 1)
	xv7001_Init();
	#endif
	
	thread_init();
	all_init = 1;
	
	#if(0)
  /* add user code end 2 */

  while(1)
  {
    /* add user code begin 3 */

    /* add user code end 3 */
  }
}

  /* add user code begin 4 */
	#endif
}
  /* add user code end 4 */
