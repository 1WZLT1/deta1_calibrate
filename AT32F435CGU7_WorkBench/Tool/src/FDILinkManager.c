#include "FDILinkManager.h"
#include "queue.h"
#include "FDILink.h"
#include "uart_serve.h"
#include "imu_task.h"

#include "rtthread.h"
#include "at32f435_437_int.h"

#include <stdint.h>
#include <wk_dma.h>

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

#define USE_EXRAWDATA 1

__packed typedef struct
{
	int count;                 //0
	float Gyroscope_X_1     ;  //1
	float Gyroscope_Y_1     ;  //2
	float Gyroscope_Z_1     ;  //3
	float Accelerometer_X_1 ;  //4
	float Accelerometer_Y_1 ;  //5
	float Accelerometer_Z_1 ;  //6
	float IMU_Temperature_1 ;  
	float Gyroscope_X_2     ;  //7
	float Gyroscope_Y_2     ;  //8
	float Gyroscope_Z_2     ;  //9
	float Accelerometer_X_2 ;  //10
	float Accelerometer_Y_2 ;  //11
	float Accelerometer_Z_2 ;  //12
	float IMU_Temperature_2 ;  
	float Gyroscope_X_3     ;  //13
	float Gyroscope_Y_3     ;  //14
	float Gyroscope_Z_3     ;  //15
	float Accelerometer_X_3 ;  //16
	float Accelerometer_Y_3 ;  //17
	float Accelerometer_Z_3 ;  //18
	float IMU_Temperature_3 ;
	float IMU_DT            ;
	timestamp_t Timestamp ;    //19
	uint32_t ChipID[4]    ;    //21
}FDILINK_COMPONENT_FB;
typedef FDILINK_COMPONENT_FB FDILink_RawData2;

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
struct rt_semaphore imuSensorPack;

#define IMU_COUNT 2

void FDILinkSend_RAWData(uint64_t time)
{
		FDIData.ExRawData.Timestamp = time;
		FDIData.ExRawData.IMU_DT = imuData.td->v2;
		FDIData.ExRawData.count = IMU_COUNT;
	
	#if IMU_COUNT >= 1
		FDIData.ExRawData.Gyroscope_X_1 = imuData.raw_gyros_1[0];
		FDIData.ExRawData.Gyroscope_Y_1 = imuData.raw_gyros_1[1];
		FDIData.ExRawData.Gyroscope_Z_1 = imuData.raw_gyros_1[2];
		FDIData.ExRawData.Accelerometer_X_1 = imuData.raw_accs_1[0];
		FDIData.ExRawData.Accelerometer_Y_1 = imuData.raw_accs_1[1];
		FDIData.ExRawData.Accelerometer_Z_1 = imuData.raw_accs_1[2];
		FDIData.ExRawData.IMU_Temperature_1 = imuData.raw_temp_1;
	#endif
	#if IMU_COUNT >= 2
		FDIData.ExRawData.Gyroscope_X_2 = imuData.raw_gyros_2[0];
		FDIData.ExRawData.Gyroscope_Y_2 = imuData.raw_gyros_2[1];
		FDIData.ExRawData.Gyroscope_Z_2 = imuData.raw_gyros_2[2];
		FDIData.ExRawData.Accelerometer_X_2 = imuData.raw_accs_2[0];
		FDIData.ExRawData.Accelerometer_Y_2 = imuData.raw_accs_2[1];
		FDIData.ExRawData.Accelerometer_Z_2 = imuData.raw_accs_2[2];
		FDIData.ExRawData.IMU_Temperature_2 = imuData.raw_temp_2;
	#endif
	
	ALIGN(8) static uint8_t fdi_buffer[256];
	FDILink_Pack(fdi_buffer, &FDILink_Handle, 0xFB, (void*)&FDIData.ExRawData, sizeof(FDILink_RawData2));
	Queue_Input(&FDIData.TxQueue,(void*)fdi_buffer, sizeof(FDILink_RawData2) + 8);
	
	rt_sem_release(&imuSensorPack);	
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
		rt_sem_take(&imuSensorPack, RT_WAITING_FOREVER);
		
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

ALIGN(8)
uint8_t Serial_Task_Stack[2048];
static struct rt_thread Serial_Task;

void FDILinkManager_Init(void)
{
	uint16_t code = GetDEVID();
	
	uint32_t* ChipID = (uint32_t*)0x1FFFF7E8;

	#if USE_EXRAWDATA
		FDIData.ExRawData.ChipID[0] = ChipID[0];
		FDIData.ExRawData.ChipID[1] = ChipID[1];
		FDIData.ExRawData.ChipID[2] = ChipID[2];
		FDIData.ExRawData.ChipID[3] = code;
	#else
		FDIData.RawData.ChipID[0] = ChipID[0];
		FDIData.RawData.ChipID[1] = ChipID[1];
		FDIData.RawData.ChipID[2] = ChipID[2];
		FDIData.RawData.ChipID[3] = code;
	#endif
	
	Queue_Init(&FDIData.TxQueue, &FDIData.TxQueueBuffer[0], 512);
	
	rt_thread_init(&Serial_Task,"Serial_Task",FDILinkSendCode,NULL,&Serial_Task_Stack[0],sizeof(Serial_Task_Stack),9,100);
	rt_thread_startup(&Serial_Task);
	rt_sem_init(&imuSensorPack,  "", 0, 0);
}

