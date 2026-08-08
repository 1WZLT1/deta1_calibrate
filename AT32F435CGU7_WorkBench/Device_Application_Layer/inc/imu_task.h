#ifndef __imu_task_h
#define __imu_task_h

#include <stdint.h>
#include "rtthread.h"
#include "TD.h"

#define IMU_SENSOR_HIST 50

typedef struct {
	/***************************************************/
	/* IMU_1 所有的产品都包含该数据 ********************/
	/***************************************************/
	float raw_accs_1[3], raw_gyros_1[3], raw_temp_1;
	int valid_accs_1[3], valid_gyros_1[3], valid_temp_1;
	/***************************************************/
	/* IMU_2 只有DETA-30和EPSILON包含该数据 ************/
	/***************************************************/
	#if defined EPSILON_SERIES || HW_VER == HW_ID_IMU_VF405_V3_688_1111
	float raw_accs_2[3], raw_gyros_2[3], raw_temp_2;
	int valid_accs_2[3], valid_gyros_2[3], valid_temp_2;
	#endif
	/***************************************************/
	/* IMU_3 只有DETA-30包含该数据 *********************/
	/***************************************************/
	#if HW_VER == HW_ID_IMU_VF405_V3_688_1111
	float raw_accs_3[3], raw_gyros_3[3], raw_temp_3;
	int valid_accs_3[3], valid_gyros_3[3], valid_temp_3;
	#endif
	/***************************************************/
	/* MAG_1 所有的产品都包含该数据 ********************/
	/***************************************************/
	float raw_mags_1[3];
	int valid_mags_1[3];
	/***************************************************/
	/* MAG_2 只有DETA-30包含该数据 *********************/
	/***************************************************/
	#if HW_VER == HW_ID_IMU_VF405_V3_688_1111
	float raw_mags_2[3];
	int valid_mags_2[3];
	#endif
	/***************************************************/
	/* PRES_1 只有DETA-30和EPSILON包含该数据 ***********/
	/***************************************************/
	#if defined EPSILON_SERIES || HW_VER == HW_ID_IMU_VF405_V3_688_1111
	float raw_pres_1;
	float raw_pres_temp_1;
	int valid_pres_1;
	#endif
	/***************************************************/
	/* 其他 ********************************************/
	/***************************************************/
    float sinRot, cosRot;
    rt_sem_t sensorFlag;
	
	volatile int calib_mode;
	//当前温度与校准时温度的差
	float temp1,temp2,temp3;
	//当前温度的变化率
	float dTemp;
    uint64_t lastUpdate;
    uint32_t dUpdateTime;
	uint64_t count;
	//TD
	TD_Param* td;
	float temp;
  float accHist[3][IMU_SENSOR_HIST];
  float gyoHist[3][IMU_SENSOR_HIST];
  float sumAcc[3];
  float sumGyo[3];
  float accHist2[3][IMU_SENSOR_HIST];
  float gyoHist2[3][IMU_SENSOR_HIST];
  float sumAcc2[3];
  float sumGyo2[3];
	int sensorHistIndex;
} imuStruct_t __attribute__ ((aligned (8))) ;

extern imuStruct_t imuData;

void imu_task_init(void);
void IMU_Handler(void);

#endif
