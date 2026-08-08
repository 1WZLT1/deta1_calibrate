#include "FDILinkManager.h"
#include "queue.h"
#include <stdint.h>
#include <wk_dma.h>
#include "FDILink.h"
#include "rtthread.h"

#include "uart_serve.h"

ALIGN(8)
uint8_t fdilink_stack[512];
static struct rt_thread fdilink_thread;

#ifndef FDILINK_STNC_BUFEER_SIZE
#define FDILINK_STNC_BUFEER_SIZE 400
#endif

typedef uint64_t timestamp_t;
__packed typedef struct
{
	float Gyroscope_X     ;
	float Gyroscope_Y     ;
	float Gyroscope_Z     ;
	float Accelerometer_X ;
	float Accelerometer_Y ;
	float Accelerometer_Z ;
	float Magnetometer_X  ;
	float Magnetometer_Y  ;
	float Magnetometer_Z  ;
	float IMU_Temperature ;
	float IMU_DT          ;
	timestamp_t Timestamp ;
	uint32_t ChipID[4]    ;
}FDILINK_COMPONENT_F3_B;

typedef FDILINK_COMPONENT_F3_B FDILink_RawData;

typedef struct
{
	Queue_t              TxQueue;
	uint8_t              TxQueueBuffer[FDILINK_STNC_BUFEER_SIZE * 2];
	#if SIMULINK_MODE != 5
		#if USE_EXRAWDATA == 0
			FDILink_RawData RawData;
		#else
			FDILink_RawData2    ExRawData;
		#endif
	#endif
	uint8_t              FDILinkSendBuffer[FDILINK_STNC_BUFEER_SIZE];
}FDILinkManager_Status_t;
static FDILinkManager_Status_t FDIData;

FDILink_t FDILink_Handle;

void FDILinkSend_RAWData(uint64_t time)
{
	FDIData.RawData.Accelerometer_X = 1;
	FDIData.RawData.Accelerometer_Y = 2;
	FDIData.RawData.Accelerometer_Z = 3;
	FDIData.RawData.Gyroscope_X = 4;
	FDIData.RawData.Gyroscope_Y = 5;
	FDIData.RawData.Gyroscope_Z = 6;
	FDIData.RawData.Magnetometer_X = 7;
	FDIData.RawData.Magnetometer_Y = 8;
	FDIData.RawData.Magnetometer_Z = 9;
	FDIData.RawData.IMU_Temperature = 10;
	FDIData.RawData.Timestamp = time;
	
	ALIGN(8) static uint8_t fdi_buffer[256];
	FDILink_Pack(fdi_buffer, &FDILink_Handle, 0xF3, (uint8_t*)&FDIData.RawData, sizeof(FDILink_RawData));
	Queue_Input(&FDIData.TxQueue,(void*)fdi_buffer, sizeof(FDILink_RawData) + 8);
}


typedef struct
{
  uint32_t IDCODE;  /*!< MCU device ID code,               Address offset: 0x00 */
  uint32_t CR;      /*!< Debug MCU configuration register, Address offset: 0x04 */
  uint32_t APB1FZ;  /*!< Debug MCU APB1 freeze register,   Address offset: 0x08 */
  uint32_t APB2FZ;  /*!< Debug MCU APB2 freeze register,   Address offset: 0x0C */
}DBGMCU_TypeDef;

#define DBGMCU              ((DBGMCU_TypeDef *) 0xE0042000)
#define IDCODE_DEVID_MASK    0x00000FFFU

uint16_t GetDEVID()
{
	return((DBGMCU->IDCODE) & IDCODE_DEVID_MASK);
}

int busy_flag = 0,count = 0;
void FDILinkSendCode(void *unused)
{
	while(1)
	{
		int len = Queue_Output(&FDIData.TxQueue, &FDIData.FDILinkSendBuffer[0], FDILINK_STNC_BUFEER_SIZE);
		if(len > 0)
		{
			while(busy_flag)
			{
					rt_thread_delay(1);
					if(count++ >= 5000)
					{
						busy_flag = 0;
					}
			}
			
			busy_flag = 1;
			
			wk_dma_channel_config(
														DMA1_CHANNEL2, 
														(uint32_t)&USART1->dt, 
														(uint32_t)(uint32_t*)&FDIData.FDILinkSendBuffer[0], 
														len
													);
			dma_channel_enable(DMA1_CHANNEL2, TRUE);
		}
	}
}



void FDILinkManager_Init(void)
{
	uint16_t code = GetDEVID();
	
	uint32_t* ChipID = (uint32_t*)0x1FFFF7E8;

	FDIData.RawData.ChipID[0] = ChipID[0];
	FDIData.RawData.ChipID[1] = ChipID[1];
	FDIData.RawData.ChipID[2] = ChipID[2];
	FDIData.RawData.ChipID[3] = code;
	
	Queue_Init(&FDIData.TxQueue, &FDIData.TxQueueBuffer[0], 512);
	
	rt_thread_init(&fdilink_thread, "FDILinkManager", FDILinkSendCode, 0, fdilink_stack,sizeof(fdilink_stack), 9, 10);
	rt_thread_startup(&fdilink_thread);
	
}

