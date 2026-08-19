/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2016-2016 InvenSense Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively “Software”) is subject
 * to InvenSense and its licensors' intellectual property rights under U.S. and international copyright
 * and other intellectual property rights laws.
 *
 * InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 * and any use, reproduction, disclosure or distribution of the Software without an express license agreement
 * from InvenSense is strictly prohibited.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE SOFTWARE IS
 * PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * INVENSENSE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 * ________________________________________________________________________________________________________
 */
#include "algo_eapi.h"
#include "Invn/LibAlgo/LibAlgo.h"
#include "Invn/EmbUtils/ErrorHelper.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

/* Ak09915 sensitivity configuration */
#define MAG_SENSITIVITY (int32_t) (9830) /* = 0.15f * (1 << 16) [uT/LSB] */

#define DATA_ACCURACY_MASK  ((uint32_t)0x7)
/* factor to convert degree to radian expressed in q30 */
#define FACTOR_DPS_TO_RPS   (int32_t) 18740330 // ((PI / 180) * (1 << 30))

/* INT20_MAX used when high resolution enable */
#define INT20_MAX (INT32_MAX >> (32-20))

static struct {
	union {
		uint8_t buf[ALGO_INVN_CALIBRATION_ACGO_CONFIG_SIZE];
		float   flt; /* ensure correct memory alignment of the buffer */
	} C_buffer;
} sCalAcc;

static struct {
	union {
		uint8_t buf[ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE];
		float   flt; /* ensure correct memory alignment of the buffer */
	} C_buffer;
} sCalGyr;

static struct {
	union {
		uint8_t buf[ALGO_INVN_ORIENTATION_CONFIG_SIZE];
		float flt; /* ensure proper alignement */
	} C_buffer;
} sGRV;

static struct {
	union {
		uint8_t buf[ALGO_INVN_CALIBRATION_CCGO_CONFIG_SIZE];
		float flt; /* ensure correct memory alignment */
	} C_buffer;
} sCalMag;

static struct {
	union {
		uint8_t buf[ALGO_INVN_ORIENTATION_CONFIG_SIZE];
		float flt; /* ensure correct memory alignment */
	} C_buffer;
} sRV;

/*
 * Variable keeping track of chip information
 */
static uint8_t chip_info[3];

/*
 * Variable keeping track of whether algorithms can used magnetometer
 */
static uint8_t mag_is_available = 0;

/*
 * Accelerometer and gyroscope sensitivities
 */
static int32_t acc_sensitivity_q16 = 0;
static int32_t gyro_sensitivity_q16 = 0;
static int32_t temp_sensitivity = 0;
static int32_t temp_offset_q16 = 0;

/* --------------------------------------------------------------------------------------
 *  Functions definition
 * -------------------------------------------------------------------------------------- */
uint8_t algorithms_init(algo_params_init * algo_parameters)
{

	/* Set accelerometer, gyroscope and temperature sensitivities */
	if(algo_parameters->is_high_res_en) {
			acc_sensitivity_q16 = ((algo_parameters->acc_fsr/1000) * (1 << 16)) / INT20_MAX;
			gyro_sensitivity_q16 = algo_parameters->gyr_fsr * (1 << 16) / INT20_MAX;

	} else {
			acc_sensitivity_q16 = ((algo_parameters->acc_fsr/1000) * (1 << 16)) / INT16_MAX;
			gyro_sensitivity_q16 = algo_parameters->gyr_fsr * (1 << 16) / INT16_MAX;
	}

	temp_sensitivity = algo_parameters->temp_sensitivity;
	temp_offset_q16 = algo_parameters->temp_offset << 16;

	int32_t gyro_offset_2000dps_q30[3] = {
		((algo_parameters->gyr_bias_q16[0] << 3) / 2000) << (30 - 16 - 3),
		((algo_parameters->gyr_bias_q16[1] << 3) / 2000) << (30 - 16 - 3),
		((algo_parameters->gyr_bias_q16[2] << 3) / 2000) << (30 - 16 - 3)
	};
	int32_t acc_offset_q25[3] = {
		(algo_parameters->acc_bias_q16[0]) << (25 - 16),
		(algo_parameters->acc_bias_q16[1]) << (25 - 16),
		(algo_parameters->acc_bias_q16[2]) << (25 - 16)
	};
	uint32_t mag_period_us;

	memcpy(chip_info, algo_parameters->params, sizeof(chip_info));

	/*
	 * Init the gyroscope calibration
	 */
	Algo_InvnCalibration_GyroCalibrationFxp_Init(sCalGyr.C_buffer.buf, gyro_offset_2000dps_q30, 0);
	Algo_InvnCalibration_GyroCalibrationFxp_SetUserParam(sCalGyr.C_buffer.buf, MOBILE_MODE);
	
	
	/* 
	 * Init the accelerometer calibration 
	 */
	Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_Init(sCalAcc.C_buffer.buf, acc_offset_q25, 0, DEFAULT_ODR_US);

	/*
	 * Init the GRV orientation
	 */
	Algo_InvnOrientation_BodyToWorldFrameFxp_Init(sGRV.C_buffer.buf, MOBILE_MODE);

	/* Mag is not present */
	if (!algo_parameters->mag_bias_q16) {
		mag_is_available = 0;
		return 0;
	}
	mag_is_available = 1;

	/*
	 * Init the compass calibration algorithm, 
	 * mag_period_us is trusted to be correctly given by caller
	 */
	mag_period_us = DEFAULT_ODR_US;
	Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_Init(sCalMag.C_buffer.buf, algo_parameters->mag_bias_q16, 0, mag_period_us);

	/*
	 * Init the RV orientation
	 */
	Algo_InvnOrientation_BodyToWorldFrameFxp_Init(sRV.C_buffer.buf, MOBILE_MODE);
	Algo_InvnOrientation_BodyToWorldFrameFxp_SetMagUserParams(sRV.C_buffer.buf, MOBILE_MODE);
	
	return 0;
}

void algorithms_configure_odr(uint32_t odr_us, uint32_t mag_odr_us)
{
	/* 
	 * Update algorithm parameters for Gyroscope calibration, including bias tracker
	 */
	Algo_InvnCalibration_GyroCalibrationFxp_SetSamplingPeriod(sCalGyr.C_buffer.buf, odr_us);
	
	/* 
	 * Update algorithm parameters for Accelerometer calibration
	 */
	Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_SetSamplingPeriod(sCalAcc.C_buffer.buf, odr_us);

	/* 
	 * Update algorithm parameters for Magnetometer calibration
	 */
	if (mag_is_available)
		Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_SetSamplingPeriod(sCalMag.C_buffer.buf, mag_odr_us);

	/* 
	 * Update algorithm parameters for GRV orientation
	 */
	Algo_InvnOrientation_BodyToWorldFrameFxp_SetGyrSamplingPeriod(sGRV.C_buffer.buf, odr_us, chip_info);
	Algo_InvnOrientation_BodyToWorldFrameFxp_SetAccSamplingPeriod(sGRV.C_buffer.buf, odr_us, chip_info);
	
	/* 
	 * Update algorithm parameters for RV orientation
	 */
	if (mag_is_available) {
		Algo_InvnOrientation_BodyToWorldFrameFxp_SetGyrSamplingPeriod(sRV.C_buffer.buf, odr_us, chip_info);
		Algo_InvnOrientation_BodyToWorldFrameFxp_SetAccSamplingPeriod(sRV.C_buffer.buf, odr_us, chip_info);
		Algo_InvnOrientation_BodyToWorldFrameFxp_SetMagSamplingPeriod(sRV.C_buffer.buf, mag_odr_us);
	}
}

void algorithms_sensor_control(uint32_t enable)
{
	/* Handling of Game Rotation Vector (6-axis AG) */
	if (enable) {
		/* Handles the orientation algoritm state */
		Algo_InvnOrientation_BodyToWorldFrameFxp_ResetStates(sGRV.C_buffer.buf);
		Algo_InvnOrientation_BodyToWorldFrameFxp_AG_Enable(sGRV.C_buffer.buf);
	} else
		Algo_InvnOrientation_BodyToWorldFrameFxp_AG_Disable(sGRV.C_buffer.buf);

	if (mag_is_available) {
		/* Handling of Rotation Vector (9-axis AGM) */
		if (enable) {
			/* Handles the orientation algoritm state */
			Algo_InvnOrientation_BodyToWorldFrameFxp_ResetStates(sRV.C_buffer.buf);
			Algo_InvnOrientation_BodyToWorldFrameFxp_AGM_Enable(sRV.C_buffer.buf);
		} else
			Algo_InvnOrientation_BodyToWorldFrameFxp_AGM_Disable(sRV.C_buffer.buf);
	}
}

void algorithms_process(algo_input *inputs, algo_output *outputs)
{
	/*
	 * Compute the calibrated temperature data
	 */
	{
		/* Get the temperature value by applying the sensitivity and adding 25degC offset */
		outputs->temp_degC_q16 = (int32_t)(((int64_t)(inputs->sRtemp_data << 16)*100) / temp_sensitivity) + temp_offset_q16;
	}

	/* Needed parameter for accelerometer and gyroscope calibration */
	const int32_t temp_100degC = (int32_t)(((int64_t)outputs->temp_degC_q16 * 100) >> 16);

	/*
	 * Compute the calibrated accelerometer data
	 */
	if (outputs->acc_accuracy_flag != -1)
	{
		int32_t raw_accel_q25[3];

		raw_accel_q25[0] = (inputs->sRacc_data[0] * acc_sensitivity_q16) << (25 - 16);
		raw_accel_q25[1] = (inputs->sRacc_data[1] * acc_sensitivity_q16) << (25 - 16);
		raw_accel_q25[2] = (inputs->sRacc_data[2] * acc_sensitivity_q16) << (25 - 16);

		int32_t accel_bias_q25[3];
		int32_t accel_cal_q25[3];

		Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_UpdateAcc(sCalAcc.C_buffer.buf, raw_accel_q25, temp_100degC, accel_bias_q25);
		accel_cal_q25[0] = raw_accel_q25[0] - accel_bias_q25[0];
		accel_cal_q25[1] = raw_accel_q25[1] - accel_bias_q25[1];
		accel_cal_q25[2] = raw_accel_q25[2] - accel_bias_q25[2];

		outputs->acc_bias_q16[0] = accel_bias_q25[0] >> (25 - 16);
		outputs->acc_bias_q16[1] = accel_bias_q25[1] >> (25 - 16);
		outputs->acc_bias_q16[2] = accel_bias_q25[2] >> (25 - 16);
		outputs->acc_cal_q16[0] = accel_cal_q25[0] >> (25 - 16);
		outputs->acc_cal_q16[1] = accel_cal_q25[1] >> (25 - 16);
		outputs->acc_cal_q16[2] = accel_cal_q25[2] >> (25 - 16);
		outputs->acc_accuracy_flag = Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_GetAccuracy(sCalAcc.C_buffer.buf);

		/* Update GRV algorithm with acc value and acc accuracy */
		Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateAcc(sGRV.C_buffer.buf, 
				accel_cal_q25, outputs->acc_accuracy_flag);

		if (mag_is_available) {
			/* Update RV algorithm with acc value and acc accuracy */
			Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateAcc(sRV.C_buffer.buf,
					accel_cal_q25, outputs->acc_accuracy_flag);
		}
	}

	/*
	 * Compute the calibrated gyroscope data
	 */
	if (outputs->gyr_accuracy_flag != -1)
	{
		int32_t gyro_cal_2000dps_q30 [3] = {0};
		int32_t raw_gyr_2000dps_q15[3] = {0};
		
		raw_gyr_2000dps_q15[0] = ((inputs->sRgyro_data[0] * gyro_sensitivity_q16) >> (16 - 15) ) / (2000);
		raw_gyr_2000dps_q15[1] = ((inputs->sRgyro_data[1] * gyro_sensitivity_q16) >> (16 - 15) ) / (2000);
		raw_gyr_2000dps_q15[2] = ((inputs->sRgyro_data[2] * gyro_sensitivity_q16) >> (16 - 15) ) / (2000);

		int32_t gyro_uncal_2000dps_q30[3];
		int32_t gyro_offset_2000dps_q30[3];

		Algo_InvnCalibration_GyroCalibrationFxp_UpdateGyr(sCalGyr.C_buffer.buf, raw_gyr_2000dps_q15, temp_100degC);
		
		Algo_InvnCalibration_GyroCalibrationFxp_GetUncalibrated(sCalGyr.C_buffer.buf, gyro_uncal_2000dps_q30);
		outputs->gyr_uncal_q16[0] = ((gyro_uncal_2000dps_q30[0] >> 11) * 2000) >> (30 - 11 - 16);
		outputs->gyr_uncal_q16[1] = ((gyro_uncal_2000dps_q30[1] >> 11) * 2000) >> (30 - 11 - 16);
		outputs->gyr_uncal_q16[2] = ((gyro_uncal_2000dps_q30[2] >> 11) * 2000) >> (30 - 11 - 16);

		Algo_InvnCalibration_GyroCalibrationFxp_GetBias(sCalGyr.C_buffer.buf, gyro_offset_2000dps_q30);

		Algo_InvnCalibration_GyroCalibrationFxp_GetCalibrated(sCalGyr.C_buffer.buf, gyro_cal_2000dps_q30);
		
		outputs->gyr_bias_q16[0] = ((gyro_offset_2000dps_q30[0] >> 11) * 2000) >> (30 - 11 - 16);
		outputs->gyr_bias_q16[1] = ((gyro_offset_2000dps_q30[1] >> 11) * 2000) >> (30 - 11 - 16);
		outputs->gyr_bias_q16[2] = ((gyro_offset_2000dps_q30[2] >> 11) * 2000) >> (30 - 11 - 16);
		
		outputs->gyr_cal_q16[0] = outputs->gyr_uncal_q16[0] - outputs->gyr_bias_q16[0];
		outputs->gyr_cal_q16[1] = outputs->gyr_uncal_q16[1] - outputs->gyr_bias_q16[1];
		outputs->gyr_cal_q16[2] = outputs->gyr_uncal_q16[2] - outputs->gyr_bias_q16[2];

		outputs->gyr_accuracy_flag = Algo_InvnCalibration_GyroCalibrationFxp_GetAccuracy(sCalGyr.C_buffer.buf);

		/* Update GRV with gyr value and gyr accuracy */
		Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateGyr(sGRV.C_buffer.buf, 
				gyro_cal_2000dps_q30, outputs->gyr_accuracy_flag);
		/* Update RV with gyr value and gyr accuracy */
		if (mag_is_available)
			Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateGyr(sRV.C_buffer.buf, 
					gyro_cal_2000dps_q30, outputs->gyr_accuracy_flag);
	}
	
	/*
	 * Compute the game rotation vector data
	 * Note : the orientation may drift until the GRV accuracy flag reaches 3. Once calibrated, the position is kept as initial reference.
	 */
	if (outputs->acc_accuracy_flag != -1 && outputs->gyr_accuracy_flag != -1) {
		Algo_InvnOrientation_BodyToWorldFrameFxp_GetGameRotationVector(sGRV.C_buffer.buf, outputs->grv_quat_q30);
	}

	/*
	 * Compute the gravity data
	 */
	if (outputs->acc_accuracy_flag != -1 && outputs->gyr_accuracy_flag != -1) {
		/* x axis */
		outputs->gravity_q16[0] = (2 * (int32_t)(((int64_t)outputs->grv_quat_q30[1] * outputs->grv_quat_q30[3]) >> 30)
				- 2 * (int32_t)(((int64_t)outputs->grv_quat_q30[0] * outputs->grv_quat_q30[2]) >> 30)) >> (30 - 16);
		/* y axis */
		outputs->gravity_q16[1] = (2 * (int32_t)(((int64_t)outputs->grv_quat_q30[2] * outputs->grv_quat_q30[3]) >> 30)
				+ 2 * (int32_t)(((int64_t)outputs->grv_quat_q30[0] * outputs->grv_quat_q30[1]) >> 30)) >> (30 - 16);
		/* z axis */
		outputs->gravity_q16[2] = ((1 << 30) - 2 * (int32_t)(((int64_t)outputs->grv_quat_q30[1] * outputs->grv_quat_q30[1]) >> 30)
				- 2 * (int32_t)(((int64_t)outputs->grv_quat_q30[2] * outputs->grv_quat_q30[2]) >> 30)) >> (30 - 16);
	}

	/*
	 * Compute the linear acceleration data
	 */
	if (outputs->acc_accuracy_flag != -1 && outputs->gyr_accuracy_flag != -1) {
		outputs->linearacc_q16[0] = outputs->acc_cal_q16[0] - outputs->gravity_q16[0];
		outputs->linearacc_q16[1] = outputs->acc_cal_q16[1] - outputs->gravity_q16[1];
		outputs->linearacc_q16[2] = outputs->acc_cal_q16[2] - outputs->gravity_q16[2];
	}

	/*
	 * Compute the rotation vector data
	 */
	if (mag_is_available) {
		int32_t heading_accuracy_q27 = Algo_InvnOrientation_BodyToWorldFrameFxp_GetRotationVector(sRV.C_buffer.buf,
			outputs->rv_quat_q30);
		outputs->rv_accuracy = (int32_t)(((int64_t)heading_accuracy_q27 * (3754936 /* 180/pi * 2^16) */)) >> 27);

		/*
		 * Compute the calibrated magnetometer data
		 */
		if (outputs->mag_accuracy_flag != -1) {
			
			const int32_t soft_iron[9] = { 1, 0, 0,
										   0, 1, 0,
										   0, 0, 1 };
			int32_t raw_mag_ut_q16[3];
			unsigned i;

			for(i = 0; i < 3; i++) {
				raw_mag_ut_q16[i]  = (int32_t)((int64_t)soft_iron[3*i+0]*inputs->sRmag_data[0]);
				raw_mag_ut_q16[i] += (int32_t)((int64_t)soft_iron[3*i+1]*inputs->sRmag_data[1]);
				raw_mag_ut_q16[i] += (int32_t)((int64_t)soft_iron[3*i+2]*inputs->sRmag_data[2]);
				raw_mag_ut_q16[i] *= MAG_SENSITIVITY;
			}

			Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_UpdateMag(sCalMag.C_buffer.buf, raw_mag_ut_q16, outputs->mag_bias_q16);

			outputs->mag_uncal_q16[0] = raw_mag_ut_q16[0];
			outputs->mag_uncal_q16[1] = raw_mag_ut_q16[1];
			outputs->mag_uncal_q16[2] = raw_mag_ut_q16[2];
			outputs->mag_cal_q16[0] = outputs->mag_uncal_q16[0] - outputs->mag_bias_q16[0];
			outputs->mag_cal_q16[1] = outputs->mag_uncal_q16[1] - outputs->mag_bias_q16[1];
			outputs->mag_cal_q16[2] = outputs->mag_uncal_q16[2] - outputs->mag_bias_q16[2];
			outputs->mag_accuracy_flag = Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_GetAccuracy(sCalMag.C_buffer.buf);

			/* Update RV with mag values and mag accuracy */
			Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateMag(sRV.C_buffer.buf,
					outputs->mag_cal_q16, outputs->mag_accuracy_flag, 0 /*the local magnetic field norm is not used for RV*/);
		}
	}
}
