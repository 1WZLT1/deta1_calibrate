#ifndef __imu_task_h
#define __imu_task_h

#include <stdint.h>
#include "rtthread.h"
#include "TD.h"

#define IMU_SENSOR_HIST 50

typedef struct {
	float accs[3], gyros[3], mags[3], pres, pres_temp, temp;
	float raw_accs[3], raw_gyros[3], raw_mags[3], raw_pres, raw_pres_temp, raw_temp;
	float sum_accs[3], sum_gyros[3], sum_mags[3], sum_pres, sum_pres_temp, sum_temp;
	float mid_gyros[3], mid_gyros_temp[3];
	float mid_accs[3], mid_accs_temp[3];
	float sum_raw_accs[3], sum_raw_gyros[3], sum_raw_mags[3], sum_raw_pres, sum_raw_pres_temp, sum_raw_temp;
	int valid_accs[3], valid_gyros[3], valid_mags[3], valid_temp, valid_pres;

	float raw_accs_1[3], raw_gyros_1[3], raw_temp_1;
	float filter_accs_1[3], filter_gyros_1[3], filter_temp_1;
	float calib_accs_1[3], calib_gyros_1[3], calib_temp_1;
	float temp_accs_1[3], temp_gyros_1[3];
	int valid_accs_1[3], valid_gyros_1[3], valid_temp_1;

	#if 1
	float sum_raw_accs_2[3], sum_raw_gyros_2[3] ,sum_raw_temp_2;
	float raw_accs_2[3], raw_gyros_2[3], raw_temp_2;
	float filter_accs_2[3], filter_gyros_2[3], filter_temp_2;
	float calib_accs_2[3], calib_gyros_2[3], calib_temp_2;
	float temp_accs_2[3], temp_gyros_2[3];
	int valid_accs_2[3], valid_gyros_2[3], valid_temp_2;
	#endif
	float raw_mags_1[3], filter_mags_1[3], calib_mags_1[3];
	int valid_mags_1[3];

	#if PRES_NUM >= 1
	float raw_pres_1, filter_pres_1, calib_pres_1;
	float raw_pres_temp_1, filter_pres_temp_1, calib_pres_temp_1;
	int valid_pres_1;
	#endif

  float sinAlgnRoll, cosAlgnRoll;
	float sinAlgnPitch, cosAlgnPitch;
	float sinAlgnYaw, cosAlgnYaw;
  rt_sem_t sensorFlag;
	int reboot_count;

	float temp1,temp2,temp3;

	float dTemp;

	float last_temp;
  int64_t lastUpdate;
  int32_t dUpdateTime;
	uint64_t count;
	uint32_t wait_time;
	uint32_t wait_count;
	float flipSign[3];
	int flip;
	float raw_acc_use_for_tare[3];
	float IMU_To_Body_Algn_Quaternion[4];
	#ifdef ERROR_STATE_KF
	double Body_To_Vehicle_Algn_Quaternion[4];
	#else
	float Body_To_Vehicle_Algn_Quaternion[4];
	#endif
	float IMU_LEVEL_Algn_Quaternion[4];


	int8_t gyro_switch_flag; //z????????,1?????,???????
	float imu_high_data_raw[3];
	float imu_high_data_calib[3];
	int8_t SCL3300_Valid;
	int8_t data_to_param_flag;
		//TD
	TD_Param* td;
} imuStruct_t __attribute__ ((aligned (8))) ;

extern imuStruct_t imuData;

void imu_task_init(void);
void IMU_Handler(void);

#endif
