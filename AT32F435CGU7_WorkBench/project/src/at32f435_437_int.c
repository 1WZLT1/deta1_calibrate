/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f435_437_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f435_437_int.h"
/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "imu_task.h"
#include "rtthread.h"
#include "imu_task.h"
#include "rtio.h"
#include "LSM6DSR.h"
#include "FDILinkManager.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */
extern rtio_t  rtio_spi1;
extern uint8_t all_init;
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

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */

  /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */

  /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
//void HardFault_Handler(void)
//{
//  /* add user code begin HardFault_IRQ 0 */

////////////  /* add user code end HardFault_IRQ 0 */
//  /* go to infinite loop when hard fault exception occurs */
//  while (1)
//  {
//    /* add user code begin W1_HardFault_IRQ 0 */

////////////    /* add user code end W1_HardFault_IRQ 0 */
//  }
//}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

  /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */

    /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

  /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */

    /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

  /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */

    /* add user code end W1_UsageFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
  /* add user code begin SVCall_IRQ 0 */

  /* add user code end SVCall_IRQ 0 */
  /* add user code begin SVCall_IRQ 1 */

  /* add user code end SVCall_IRQ 1 */
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */

  /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */

  /* add user code end DebugMonitor_IRQ 1 */
}

///**
//  * @brief  this function handles pendsv_handler exception.
//  * @param  none
//  * @retval none
//  */
//void PendSV_Handler(void)
//{
//  /* add user code begin PendSV_IRQ 0 */

////////////  /* add user code end PendSV_IRQ 0 */
//  /* add user code begin PendSV_IRQ 1 */

////////////  /* add user code end PendSV_IRQ 1 */
//}

///**
//  * @brief  this function handles systick handler.
//  * @param  none
//  * @retval none
//  */
//void SysTick_Handler(void)
//{
//  /* add user code begin SysTick_IRQ 0 */

////////////  /* add user code end SysTick_IRQ 0 */


//  /* add user code begin SysTick_IRQ 1 */

////////////  /* add user code end SysTick_IRQ 1 */
//}

/**
  * @brief  this function handles EXINT Line [9:5] handler.
  * @param  none
  * @retval none
  */
void EXINT9_5_IRQHandler(void)
{
  /* add user code begin EXINT9_5_IRQ 0 */
	rt_interrupt_enter();
	exint_flag_clear(EXINT_LINE_6);	
	if(all_init == 1)LSM6DSR_callback(&rtio_spi1);
	rt_interrupt_leave();
  /* add user code end EXINT9_5_IRQ 0 */
  /* add user code begin EXINT9_5_IRQ 1 */
  /* add user code end EXINT9_5_IRQ 1 */
}

/**
  * @brief  this function handles TMR2 handler.
  * @param  none
  * @retval none
  */
void TMR2_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR2_GLOBAL_IRQ 0 */
	if(TMR2->ists & 0x01)
	{
		rt_interrupt_enter();
		IMU_Handler();
		/*Moderately increase the data request frequency to compensate for the I2C wait time and keep the received data rate around 100 Hz.*/
		
		TMR2->ists &= ~(0x01);
		rt_interrupt_leave();
	}	
  /* add user code end TMR2_GLOBAL_IRQ 0 */


  /* add user code begin TMR2_GLOBAL_IRQ 1 */

  /* add user code end TMR2_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 1 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* add user code begin DMA1_Channel1_IRQ 0 */

  /* add user code end DMA1_Channel1_IRQ 0 */
  /* add user code begin DMA1_Channel1_IRQ 1 */

  /* add user code end DMA1_Channel1_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 2 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* add user code begin DMA1_Channel2_IRQ 0 */
	if(DMA1->sts_bit.fdtf2 == 1)
	{
		rt_interrupt_enter();
		
		dma_flag_clear(DMA1_FDT2_FLAG);
		dma_channel_enable(DMA1_CHANNEL2, FALSE);
		busy_flag = 0;
		rt_interrupt_leave();
	}
  /* add user code end DMA1_Channel2_IRQ 0 */
  /* add user code begin DMA1_Channel2_IRQ 1 */

  /* add user code end DMA1_Channel2_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 5 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel5_IRQHandler(void)
{
  /* add user code begin DMA1_Channel5_IRQ 0 */
	if(DMA1->sts_bit.fdtf5 == 1)
	{
		rt_interrupt_enter();
		
		dma_flag_clear(DMA1_FDT5_FLAG);
		dma_flag_clear(DMA1_GL5_FLAG);
		
		rtio_node_t *node = rtio_spi1.current_node;
		if(node != NULL)
		{
			if(rtio_spi1.current_node->Device_Name == LSM6DSR_E)
			{
				LSM6DSR_CS_disenable;
				
				rtio_node_rx_t dev;
				dev.Device_Name = LSM6DSR_E;
				rt_memcpy(&dev.rx,&LSM6DR_Data_Receive,sizeof(LSM6DR_Data_Receive));
				rt_mq_send(sensor_cqe_mq,&dev, sizeof(rtio_node_rx_t));
			}

			rtio_spi1.current_node = NULL;
			rtio_release(&rtio_spi1,node);
			rtio_spi1.Dev_busy_State = Dev_IDLE; 
		}
		
		dma_channel_enable(DMA1_CHANNEL5, FALSE);
		rt_interrupt_leave();
	}
  /* add user code end DMA1_Channel5_IRQ 0 */
  /* add user code begin DMA1_Channel5_IRQ 1 */

  /* add user code end DMA1_Channel5_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 6 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* add user code begin DMA1_Channel6_IRQ 0 */
	if(DMA1->sts_bit.fdtf6 == 1)
	{
		rt_interrupt_enter();
		dma_flag_clear(DMA1_FDT6_FLAG);
		dma_channel_enable(DMA1_CHANNEL6, FALSE);
		rt_interrupt_leave();
	}
  /* add user code end DMA1_Channel6_IRQ 0 */
  /* add user code begin DMA1_Channel6_IRQ 1 */

  /* add user code end DMA1_Channel6_IRQ 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
