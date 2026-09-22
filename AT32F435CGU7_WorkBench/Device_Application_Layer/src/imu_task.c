#include "imu_task.h"
#include "sensor.h"
#include "xv7001.h"
#include "SCHA16T.h"
#include "icm42688.h"
#include "lsm6dsr.h"
#include "FDIlinkManager.h"

#include "at32f435_437_int.h"
#include "rtthread.h"

#define IMU_PRIORITY	    4

#if(DT_HardWare_Version != 0x0001040000)
#define AccChipToBody(b,c)	  do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)
#define GyroChipToBody(b,c)	  do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)

#define Acc2ChipToBody(b,c)	   do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)
#define Gyro2ChipToBody(b,c)   do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)
#else 
#define AccChipToBody(b,c)	  do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)
#define GyroChipToBody(b,c)	  do{b[0] = c[1];b[1] = c[0];b[2] = -c[2];}while(0)

#define Acc2ChipToBody(b,c)	   do{b[0] = -c[0];b[1] = c[1];b[2] = -c[2];}while(0)
#define Gyro2ChipToBody(b,c)   do{b[0] = -c[0];b[1] = c[1];b[2] = -c[2];}while(0)
#endif

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

uint64_t last_dt = 0;
uint64_t now_dt  = 0;
uint64_t space   = 0;

static void Imu_Task_Function(void* parameter)
{
	while(1)
	{
		rt_sem_take(&imuSensor, RT_WAITING_FOREVER);
		float accs_1[3];
		float gyros_1[3];
		float temperature_1;

		now_dt = Micros();
		space = now_dt - last_dt;
		last_dt = now_dt;
		
		#if IMU_COUNT >= 2
		float accs_2[3];
		float gyros_2[3];
		float temperature_2;
		#endif
		int primask = __get_PRIMASK();
		__set_PRIMASK(1);
		
		#if (DT_HardWare_Version == 0x0001000000)
		LSM6DSR_Decode(&LSM6DSR);
		{
			accs_1[0]  = lsm6dsr_out.acc_x;
			accs_1[1]  = lsm6dsr_out.acc_y;
			accs_1[2]  = lsm6dsr_out.acc_z;
			gyros_1[0] = lsm6dsr_out.gyro_x;
			gyros_1[1] = lsm6dsr_out.gyro_y;
			gyros_1[2] = lsm6dsr_out.gyro_z;
			temperature_1 = lsm6dsr_out.temp;
		}
		#elif (DT_HardWare_Version == 0x0001010000)
		{
			#if (DT_LSM6DS3TR_ENABLED == 1 || DT_LSM6DSRTR_ENABLED == 1)
				LSM6DSR_Decode(&LSM6DSR);
				accs_1[0]     = LSM6DSR.Accs[0] ;
				accs_1[1]     = LSM6DSR.Accs[1] ;
				accs_1[2]     = LSM6DSR.Accs[2] ;
				gyros_1[0]    = LSM6DSR.Gyros[0];
				gyros_1[1]    = LSM6DSR.Gyros[1];
				gyros_1[2]    = LSM6DSR.Gyros[2];
				temperature_1 = LSM6DSR.Temp    ;
			#elif (DT_IIM42652_ENABLED == 1)
				accs_1[0]  = ICM42688_BufferData.Accs[0];
				accs_1[1]  = ICM42688_BufferData.Accs[1];
				accs_1[2]  = ICM42688_BufferData.Accs[2];
				gyros_1[0] = ICM42688_BufferData.Gyros[0];
				gyros_1[1] = ICM42688_BufferData.Gyros[1];
				gyros_1[2] = ICM42688_BufferData.Gyros[2];
				temperature_1 = ICM42688_BufferData.Temp;
			#endif
		}
		#elif (DT_HardWare_Version == 0x0001020000)
		{
			#if (DT_LSM6DS3TR_ENABLED == 1 || DT_LSM6DSRTR_ENABLED == 1)
				accs_2[0]  = accs_1[0]  = lsm6dsr_out.acc_x;
				accs_2[1]  = accs_1[1]  = lsm6dsr_out.acc_y;
				accs_2[2]  = accs_1[2]  = lsm6dsr_out.acc_z;
				gyros_2[0] = gyros_1[0] = lsm6dsr_out.gyro_x;
				gyros_2[1] = gyros_1[1] = lsm6dsr_out.gyro_y;
				gyros_1[2] = lsm6dsr_out.gyro_z;
				gyros_2[2] = xv7001_x_gyro_raw;
				temperature_2 = temperature_1 = lsm6dsr_out.temp;
			#elif (DT_IIM42652_ENABLED == 1)
				accs_2[0]  = accs_1[0]  = ICM42688_BufferData.Accs[0];
				accs_2[1]  = accs_1[1]  = ICM42688_BufferData.Accs[1];
				accs_2[2]  = accs_1[2]  = ICM42688_BufferData.Accs[2];
				gyros_2[0] = gyros_1[0] = ICM42688_BufferData.Gyros[0];
				gyros_2[1] = gyros_1[1] = ICM42688_BufferData.Gyros[1];
			
				gyros_1[2] = ICM42688_BufferData.Gyros[2];
				gyros_2[2] = xv7001_x_gyro_raw;
			
				temperature_2 = temperature_1 = ICM42688_BufferData.Temp;
			#endif
			
			
		}
		#elif (DT_HardWare_Version == 0x0001030000) || (DT_HardWare_Version == 0x0001040000)
		{
			SCH1633_Decode(&SCH1633);
			accs_1[0] = SCH1633.Accs[0];
			accs_1[1] = SCH1633.Accs[1];
			accs_1[2] = SCH1633.Accs[2];
			
			gyros_1[0] = SCH1633.Gyros[0] * DEG_TO_RAD;
			gyros_1[1] = SCH1633.Gyros[1] * DEG_TO_RAD;
			gyros_1[2] = SCH1633.Gyros[2] * DEG_TO_RAD;
		
			temperature_1 = SCH1633.Temp;
			
			#if (DT_LSM6DS3TR_ENABLED == 1 || DT_LSM6DSRTR_ENABLED == 1)
			
			LSM6DSR_Decode(&LSM6DSR);
			accs_2[0]  = LSM6DSR.Accs[0];
			accs_2[1]  = LSM6DSR.Accs[1];
			accs_2[2]  = LSM6DSR.Accs[2];
			gyros_2[0] = LSM6DSR.Gyros[0];
			gyros_2[1] = LSM6DSR.Gyros[1];
			gyros_2[2] = LSM6DSR.Gyros[2];
			
			temperature_1 = LSM6DSR.Temp;
			
			#elif (DT_IIM42652_ENABLED == 1)
			accs_2[0] = ICM42688_BufferData.Accs[0];
			accs_2[1] = ICM42688_BufferData.Accs[1];
			accs_2[2] = ICM42688_BufferData.Accs[2];
			
			gyros_2[0] = ICM42688_BufferData.Gyros[0];
			gyros_2[1] = ICM42688_BufferData.Gyros[1];
			gyros_2[2] = ICM42688_BufferData.Gyros[2];
			
			temperature_2 = ICM42688_BufferData.Temp;
			
			#endif
			
		}
		#endif
		
		//数据平均滤波
		// 记录acc、mag和压力读数的历史，以便更平滑
		// 加速度计
		//减去过时数据
		imuData.sumAcc[0] -= imuData.accHist[0][imuData.sensorHistIndex];
		imuData.sumAcc[1] -= imuData.accHist[1][imuData.sensorHistIndex];
		imuData.sumAcc[2] -= imuData.accHist[2][imuData.sensorHistIndex];
		#if IMU_COUNT >= 2
		imuData.sumAcc2[0] -= imuData.accHist2[0][imuData.sensorHistIndex];
		imuData.sumAcc2[1] -= imuData.accHist2[1][imuData.sensorHistIndex];
		imuData.sumAcc2[2] -= imuData.accHist2[2][imuData.sensorHistIndex];
		#endif

		//用最新数据覆盖最老的数据
		imuData.accHist[0][imuData.sensorHistIndex] = accs_1[0];
		imuData.accHist[1][imuData.sensorHistIndex] = accs_1[1];
		imuData.accHist[2][imuData.sensorHistIndex] = accs_1[2];
		#if IMU_COUNT >= 2
		imuData.accHist2[0][imuData.sensorHistIndex] = accs_2[0];
		imuData.accHist2[1][imuData.sensorHistIndex] = accs_2[1];
		imuData.accHist2[2][imuData.sensorHistIndex] = accs_2[2];
		#endif
		
		// sum 加上最新的数据
		//减去移除队列的数据再加上新的数据重新等于队列的和
		imuData.sumAcc[0] += imuData.accHist[0][imuData.sensorHistIndex];
		imuData.sumAcc[1] += imuData.accHist[1][imuData.sensorHistIndex];
		imuData.sumAcc[2] += imuData.accHist[2][imuData.sensorHistIndex];
		#if IMU_COUNT >= 2
		imuData.sumAcc2[0] += imuData.accHist2[0][imuData.sensorHistIndex];
		imuData.sumAcc2[1] += imuData.accHist2[1][imuData.sensorHistIndex];
		imuData.sumAcc2[2] += imuData.accHist2[2][imuData.sensorHistIndex];
		#endif
		
		// 陀螺仪计
		//减去过时数据
		imuData.sumGyo[0] -= imuData.gyoHist[0][imuData.sensorHistIndex];
		imuData.sumGyo[1] -= imuData.gyoHist[1][imuData.sensorHistIndex];
		imuData.sumGyo[2] -= imuData.gyoHist[2][imuData.sensorHistIndex];
		#if IMU_COUNT >= 2
		imuData.sumGyo2[0] -= imuData.gyoHist2[0][imuData.sensorHistIndex];
		imuData.sumGyo2[1] -= imuData.gyoHist2[1][imuData.sensorHistIndex];
		imuData.sumGyo2[2] -= imuData.gyoHist2[2][imuData.sensorHistIndex];
		#endif
		//用最新数据覆盖最老的数据
		imuData.gyoHist[0][imuData.sensorHistIndex] = gyros_1[0];
		imuData.gyoHist[1][imuData.sensorHistIndex] = gyros_1[1];
		imuData.gyoHist[2][imuData.sensorHistIndex] = gyros_1[2];
		#if IMU_COUNT >= 2
		imuData.gyoHist2[0][imuData.sensorHistIndex] = gyros_2[0];
		imuData.gyoHist2[1][imuData.sensorHistIndex] = gyros_2[1];
		imuData.gyoHist2[2][imuData.sensorHistIndex] = gyros_2[2];
		#endif
		// sum 加上最新的数据
		//减去移除队列的数据再加上新的数据重新等于队列的和
		imuData.sumGyo[0] += imuData.gyoHist[0][imuData.sensorHistIndex];
		imuData.sumGyo[1] += imuData.gyoHist[1][imuData.sensorHistIndex];
		imuData.sumGyo[2] += imuData.gyoHist[2][imuData.sensorHistIndex];
		#if IMU_COUNT >= 2
		imuData.sumGyo2[0] += imuData.gyoHist2[0][imuData.sensorHistIndex];
		imuData.sumGyo2[1] += imuData.gyoHist2[1][imuData.sensorHistIndex];
		imuData.sumGyo2[2] += imuData.gyoHist2[2][imuData.sensorHistIndex];
		#endif
		
		float accs_11[3], gyros_11[3];
    accs_11[0] = 	imuData.sumAcc[0]	/ IMU_SENSOR_HIST;
		accs_11[1] = 	imuData.sumAcc[1]	/ IMU_SENSOR_HIST;
		accs_11[2] = 	imuData.sumAcc[2]	/ IMU_SENSOR_HIST;
		
		gyros_11[0] = imuData.sumGyo[0] / IMU_SENSOR_HIST;
		gyros_11[1] = imuData.sumGyo[1] / IMU_SENSOR_HIST;
		gyros_11[2] = imuData.sumGyo[2] / IMU_SENSOR_HIST;
		
		#if IMU_COUNT >= 2
		float accs_22[3], gyros_22[3];
    accs_22[0] = 	imuData.sumAcc2[0]	/ IMU_SENSOR_HIST;
		accs_22[1] = 	imuData.sumAcc2[1]	/ IMU_SENSOR_HIST;
		accs_22[2] = 	imuData.sumAcc2[2]	/ IMU_SENSOR_HIST;
		
		gyros_22[0] = imuData.sumGyo2[0] / IMU_SENSOR_HIST;
		gyros_22[1] = imuData.sumGyo2[1] / IMU_SENSOR_HIST;
		gyros_22[2] = imuData.sumGyo2[2] / IMU_SENSOR_HIST;
		#endif
		
		imuData.sensorHistIndex = (imuData.sensorHistIndex + 1) % IMU_SENSOR_HIST;
		__set_PRIMASK(primask);
		
		#if(1)
			AccChipToBody(imuData.raw_accs_1, accs_11);
			GyroChipToBody(imuData.raw_gyros_1, gyros_11);
			#if IMU_COUNT >= 2
			Acc2ChipToBody(imuData.raw_accs_2, accs_22);
			Gyro2ChipToBody(imuData.raw_gyros_2, gyros_22);
		#endif
		
		#else
			AccChipToBody(imuData.raw_accs_1, accs_1);
			GyroChipToBody(imuData.raw_gyros_1, gyros_1);
			#if IMU_COUNT >= 2
			Acc2ChipToBody(imuData.raw_accs_2, accs_2);
			Gyro2ChipToBody(imuData.raw_gyros_2, gyros_2);
			#endif 
		#endif
		
		imuData.raw_temp_1 = temperature_1;
		#if IMU_COUNT >= 2
		imuData.raw_temp_2 = temperature_2;
		#endif
		
		float thisTemp = imuData.raw_temp_1;
		float lastTemp = imuData.temp;
		float newTemp = thisTemp * TEMP_SMOOTH + (1 - TEMP_SMOOTH) * lastTemp;
		imuData.temp = newTemp;
		
		adrc_td(imuData.td, newTemp);
		
		uint64_t this_time = Micros();
		imuData.lastUpdate = this_time;
		FDILinkSend_RAWData(this_time);
		
		static uint64_t last_time = 0;
		imuData.dUpdateTime = this_time - last_time;
		last_time = this_time;
		imuData.lastUpdate = this_time;
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

