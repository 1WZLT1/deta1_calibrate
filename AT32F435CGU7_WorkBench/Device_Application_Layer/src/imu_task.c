#include "imu_task.h"
#include "sensor.h"
#include "LSM6DSR.h"
#include "FDIlinkManager.h"

#include "at32f435_437_int.h"

#include "rtthread.h"

#define IMU_PRIORITY	    9

#define AccChipToBody(b,c)	  do{b[0] = -c[1];b[1] = -c[0];b[2] = -c[2];}while(0)
#define GyroChipToBody(b,c)	  do{b[0] = -c[1];b[1] = -c[0];b[2] = -c[2];}while(0)

//重复计数错误检测
static int RawBuffer_DoubleCountErrorDetection(RawBuffer_t* buffer, int threshold)
{
	return buffer->repeat_count > threshold;
}

#define TEMP_SMOOTH 0.0113f
imuStruct_t imuData;

ALIGN(8)
uint8_t Imu_Task_Stack[2048];
static struct rt_thread Imu_Task;
struct rt_semaphore imuSensor;

int i = 0;
static void Imu_Task_Function(void* parameter)
{
	while(1)
	{
		rt_sem_take(&imuSensor, RT_WAITING_FOREVER);
		int primask = __get_PRIMASK();
		__set_PRIMASK(1);
		float accs_1[3] = {RawBuffer_Output(&LSM6DSR.BufAccX),RawBuffer_Output(&LSM6DSR.BufAccY),RawBuffer_Output(&LSM6DSR.BufAccZ)};
		float gyros_1[3] = {RawBuffer_Output(&LSM6DSR.BufGyroX),RawBuffer_Output(&LSM6DSR.BufGyroY),RawBuffer_Output(&LSM6DSR.BufGyroZ)};
		float temperature_1 = RawBuffer_Output(&LSM6DSR.BufTemp);
		int acc_error_1[3] = {RawBuffer_DoubleCountErrorDetection(&LSM6DSR.BufAccX, 50),RawBuffer_DoubleCountErrorDetection(&LSM6DSR.BufAccY, 50),RawBuffer_DoubleCountErrorDetection(&LSM6DSR.BufAccZ, 50)};
		int gyro_error_1[3] = {RawBuffer_DoubleCountErrorDetection(&LSM6DSR.BufGyroX, 50),RawBuffer_DoubleCountErrorDetection(&LSM6DSR.BufGyroY, 50),RawBuffer_DoubleCountErrorDetection(&LSM6DSR.BufGyroZ, 50)};
		int temperature_error_1 = RawBuffer_DoubleCountErrorDetection(&LSM6DSR.BufTemp, 50);
		__set_PRIMASK(primask);
		
		float accb_error_1[3], gyrob_error_1[3], magb_error_1[3];
		//传感器数据坐标轴转换
		AccChipToBody(imuData.raw_accs_1, accs_1);
		GyroChipToBody(imuData.raw_gyros_1, gyros_1);
		
		AccChipToBody(accb_error_1, acc_error_1);
		GyroChipToBody(gyrob_error_1, gyro_error_1);
		
		for(int i = 0;i < 3;i++)
		{
			//error = 1 或 -1则表示该传感器该轴是无效数据
			//error = 0则表示该传感器该轴是有效数据
			imuData.valid_accs_1[i]	 = !accb_error_1[i];
			imuData.valid_gyros_1[i] = !gyrob_error_1[i];
			imuData.valid_mags_1[i]	 = !magb_error_1[i];
		}
		
		imuData.raw_temp_1 = temperature_1;
		imuData.valid_temp_1 = !temperature_error_1;
		
		float thisTemp = imuData.raw_temp_1;
		float lastTemp = imuData.temp;
		float newTemp = thisTemp * TEMP_SMOOTH + (1 - TEMP_SMOOTH) * lastTemp;
		imuData.temp = newTemp;
		
		adrc_td(imuData.td, newTemp);
		
		uint64_t this_time = Micros();
		imuData.lastUpdate = this_time;
		FDILinkSend_RAWData(this_time);
	}
}

static volatile int IMUInit_Count = 0;
void IMU_Handler()
{
	if (IMUInit_Count == 0)				/* 检查imu任务是否初始化完成 */
		return;
	rt_sem_release(&imuSensor);	
}

extern void FDILinkSendCode(void *unused);

void imu_task_init()
{
	rt_thread_init(&Imu_Task,"Imu_Task",Imu_Task_Function,NULL,&Imu_Task_Stack[0],sizeof(Imu_Task_Stack),IMU_PRIORITY,100);
	rt_thread_startup(&Imu_Task);
	rt_sem_init(&imuSensor,  "", 0, 0);
	IMUInit_Count++;
}

