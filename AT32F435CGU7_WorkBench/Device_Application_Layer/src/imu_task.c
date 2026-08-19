#include "imu_task.h"
#include "sensor.h"
#include "xv7001.h"
#include "SCHA16T.h"
#include "icm42688.h"
#include "FDIlinkManager.h"

#include "at32f435_437_int.h"
#include "rtthread.h"

#define IMU_PRIORITY	    9

#define AccChipToBody(b,c)	  do{b[0] = -c[1];b[1] = -c[0];b[2] = -c[2];}while(0)
#define GyroChipToBody(b,c)	  do{b[0] = +c[2];b[1] = +c[1];b[2] = -c[0];}while(0)
#define Acc2ChipToBody(b,c)	  do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)
#define Gyro2ChipToBody(b,c)  do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)

#define RUN_TASK_FREQ 200
#define RUN_TASK_PERIOD (1000000/RUN_TASK_FREQ)
#define IMU_TASK_FREQ 400
#define IMU_TASK_PERIOD (1000000/IMU_TASK_FREQ)

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
		imuData.wait_time += IMU_TASK_PERIOD;
		imuData.count++;
		imuData.wait_count++;
		
		int primask = __get_PRIMASK();
		__set_PRIMASK(1);
		float accs_1[3]  = {sch16t_out.acc_x,sch16t_out.acc_y,sch16t_out.acc_z};
		float accs_2[3]  = {ICM42688_BufferData.Accs[0],ICM42688_BufferData.Accs[1],ICM42688_BufferData.Accs[2]};
		float gyros_1[3] = {sch16t_out.gyro_x,sch16t_out.gyro_y,sch16t_out.gyro_z};
		float gyros_2[3] = {ICM42688_BufferData.Gyros[0],ICM42688_BufferData.Gyros[1],ICM42688_BufferData.Gyros[2]};
		
		float temperature_1 = sch16t_out.temp;
		float temperature_2 = ICM42688_BufferData.Temp;
		
		__set_PRIMASK(primask);
		/***************************************************/
		/* 坐标系转换 **************************************/
		/***************************************************/
		
		//传感器数据坐标轴转换
		AccChipToBody(imuData.raw_accs_1, accs_1);
		Acc2ChipToBody(imuData.raw_accs_2, accs_2);
		GyroChipToBody(imuData.raw_gyros_1, gyros_1);
		Gyro2ChipToBody(imuData.raw_gyros_2, gyros_2);
		
		imuData.raw_temp_1 = temperature_1;
		imuData.raw_temp_2 = temperature_2;
		
		imuData.sum_raw_accs[0] += imuData.raw_accs_1[0];
		imuData.sum_raw_accs[1] += imuData.raw_accs_1[1];
		imuData.sum_raw_accs[2] += imuData.raw_accs_1[2];
		imuData.sum_raw_gyros[0] += imuData.raw_gyros_1[0];
		imuData.sum_raw_gyros[1] += imuData.raw_gyros_1[1];
		imuData.sum_raw_gyros[2] += imuData.raw_gyros_1[2];
		imuData.sum_raw_temp += imuData.raw_temp_1;
		
		imuData.sum_raw_accs_2[0] += imuData.raw_accs_2[0];
		imuData.sum_raw_accs_2[1] += imuData.raw_accs_2[1];
		imuData.sum_raw_accs_2[2] += imuData.raw_accs_2[2];
		imuData.sum_raw_gyros_2[0] += imuData.raw_gyros_2[0];
		imuData.sum_raw_gyros_2[1] += imuData.raw_gyros_2[1];
		imuData.sum_raw_gyros_2[2] += imuData.raw_gyros_2[2];
		imuData.sum_raw_temp_2 += imuData.raw_temp_2;
		
		if(imuData.wait_time >= RUN_TASK_PERIOD)
		{
			imuData.raw_accs[0] = imuData.sum_raw_accs[0] / imuData.wait_count;
			imuData.raw_accs[1] = imuData.sum_raw_accs[1] / imuData.wait_count;
			imuData.raw_accs[2] = imuData.sum_raw_accs[2] / imuData.wait_count;
			imuData.raw_gyros[0] = imuData.sum_raw_gyros[0] / imuData.wait_count;
			imuData.raw_gyros[1] = imuData.sum_raw_gyros[1] / imuData.wait_count;
			imuData.raw_gyros[2] = imuData.sum_raw_gyros[2] / imuData.wait_count;
			imuData.raw_temp = imuData.sum_raw_temp / imuData.wait_count;
			
			imuData.raw_accs_2[0] = imuData.sum_raw_accs_2[0] / imuData.wait_count;
			imuData.raw_accs_2[1] = imuData.sum_raw_accs_2[1] / imuData.wait_count;
			imuData.raw_accs_2[2] = imuData.sum_raw_accs_2[2] / imuData.wait_count;
			imuData.raw_gyros_2[0] = imuData.sum_raw_gyros_2[0] / imuData.wait_count;
			imuData.raw_gyros_2[1] = imuData.sum_raw_gyros_2[1] / imuData.wait_count;
			imuData.raw_gyros_2[2] = imuData.sum_raw_gyros_2[2] / imuData.wait_count;
			imuData.raw_temp_2 = imuData.sum_raw_temp_2 / imuData.wait_count;
			
			imuData.sum_raw_accs[0] = 0;
			imuData.sum_raw_accs[1] = 0;
			imuData.sum_raw_accs[2] = 0;
			imuData.sum_raw_gyros[0] = 0;
			imuData.sum_raw_gyros[1] = 0;
			imuData.sum_raw_gyros[2] = 0;
			imuData.sum_raw_temp = 0;
			
			imuData.sum_raw_accs_2[0] = 0;
			imuData.sum_raw_accs_2[1] = 0;
			imuData.sum_raw_accs_2[2] = 0;
			imuData.sum_raw_gyros_2[0] = 0;
			imuData.sum_raw_gyros_2[1] = 0;
			imuData.sum_raw_gyros_2[2] = 0;
			imuData.sum_raw_temp_2 = 0;
			
			imuData.wait_time -= RUN_TASK_PERIOD;
			imuData.wait_count = 0;
			
			uint64_t this_time = Micros();
			imuData.lastUpdate = this_time;
			FDILinkSend_RAWData(this_time);
		}
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

