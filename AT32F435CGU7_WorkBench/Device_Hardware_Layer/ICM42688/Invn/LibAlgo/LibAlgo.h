/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2016-2016 InvenSense Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively "Software") is subject
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
#ifndef _INV_LIB_ALGO_H_
#define _INV_LIB_ALGO_H_

#include "Invn/InvExport.h"
#include <stdint.h>

/**
* \brief Predefined use cases
*/
enum algo_mode {
	MOBILE_MODE,	//!< Regular mode, calibration is more sensitive to small movements
	ALGO_MODES
};

/*------ Defines -----------------------------------------------------------------*/
/*
 * These below defines are copied from algorithm configuration files
 * to avoid to much headers dependancy to install in package for few configuration used
 * /!\ Warning /!\ on algortihm version update, these defines may change!
 */
 
#define ALGO_INVN_CALIBRATION_ACGO_CONFIG_SIZE     516
#define ALGO_INVN_CALIBRATION_CCGO_CONFIG_SIZE     516
#define ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE     256
#define ALGO_INVN_ORIENTATION_CONFIG_SIZE          648
#define ALGO_INVN_GESTURE_CONFIG_SIZE              360
#define ALGO_INVN_AAR_CONFIG_SIZE                  1576

 /* Parameters for Gyroscope calibration algorithm (Mobile) */
#define	ALGO_MOBILE_GYRO_CALIBRATION_SAMPLE_NUM_LOG2							6
#define	ALGO_MOBILE_GYRO_CALIBRATION_DURATION_US								500000
#define	ALGO_MOBILE_LOW_POWER_GYRO_CALIBRATION_DATA_DIFF_THRESHOLD				20
#define	ALGO_MOBILE_LOW_POWER_GYRO_CALIBRATION_HIGH_ORDER_VARIANCE_THRESHOLD	3
#define	ALGO_MOBILE_LOW_NOISE_GYRO_CALIBRATION_DATA_DIFF_THRESHOLD				8
#define	ALGO_MOBILE_LOW_NOISE_GYRO_CALIBRATION_HIGH_ORDER_VARIANCE_THRESHOLD	2

#define	ALGO_DEFAULT_GYRO_CALIBRATION_FNM_BIAS_REJECTION_THRESHOLD				21474836L

/* Parameters for 9-axis Rotation Vector Orientation */
#define ALGO_RV_ANOMALIES_REJECTION                                1073741824L // = 100% rejection = 2^30L
#define ALGO_RV_THRESHOLD_YAW_STOP_CONVERGENCE                    -1L
#define ALGO_MOBILE_RV_THRESHOLD_GYR_STOP_CONVERGENCE             -1L 
#define ALGO_MOBILE_RV_THRESHOLD_YAW_SMOOTH_CONVERGENCE_DEFAULT   -1L


/* AccelCalibrationGyroOptionalFxp */

/**
* \brief Initializes the calibration algorithm. this function should be called at the first start of sensor but should not be called after a switch off/on of the sensor since the accelerometer offset is not subject to change during off period.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_ACGO_CONFIG_SIZE to use internally. 
* \param[in] accOffset[3] initial offset [g]. fxp format is \b q25 \n If pointer is NULL or 0, offset will be set to { 0, 0, 0},
* \param[in] accuracy accuracy of initial offset (0 to 3).
* \param[in] acc_sampling_period sampling period [us] of the accelerometer sensor. If called with acc_sampling_period==0, the previous value of sampling period will be kept.
* \warning This method sets default parameters value, it will resets the algorithm and set accuracy to 0.
* \ingroup AccelCalibrationGyroOptional
*/
void INV_EXPORT Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_Init(void* data, int32_t accOffset[3], int32_t accuracy, uint32_t acc_sampling_period);

/**
* \brief This function allows to change accel sampling period on the fly without reseting the algorithm and so keeping accuracy level.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_ACGO_CONFIG_SIZE to use internally. 
* \param[in] acc_sampling_period sampling period [us] of the accelerometer sensor. If called with acc_sampling_period==0, the previous value of sampling period will be kept.
* \warning This method will keep offset and accuracy level.
* \ingroup AccelCalibrationGyroOptional
*/
void INV_EXPORT Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_SetSamplingPeriod(void *data, uint32_t acc_sampling_period);

/**
* \brief Gets accuracy level of current offset.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_ACGO_CONFIG_SIZE to use internally.
* \retval 0 Unreliable
* \retval 1 Low
* \retval 2 Medium
* \retval 3 High
* \ingroup AccelCalibrationGyroOptional
*/
uint8_t INV_EXPORT Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_GetAccuracy(void *data);

/**
* \brief Updates the algorithm with new accelerometer sensor data.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_ACGO_CONFIG_SIZE to use internally.
* \param[in] acc_uncalib, uncalibrated accelerometer data [g]. fxp format is \b q25
* \param[in] acc_temperature, uncalibrated accelerometer temperature in °C*100
* \param[out] acc_offset[3] offset [g], coded in q25.
* \return 1 when new position has been taken into account,
* \return 2 when a new offset has been validated,
* \return 0 otherwise.
* \ingroup AccelCalibrationGyroOptional
*/
int32_t INV_EXPORT Algo_InvnCalibration_AccelCalibrationGyroOptionalFxp_UpdateAcc(void* data, const int32_t acc_uncalib[3], const int32_t acc_temperature, int32_t acc_offset[3]);


/* GyroCalibration */

/**
* \brief Initializes the calibration algorithm.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally. 
* \param[in] gyro_bias initial bias, fxp format is \b 2^30 = 2000dps. \n If pointer is NULL or 0, bias will be set to { 0, 0, 0}.
* \param[in] accuracy accuracy level of current bias.
* \warning This method sets default parameters value.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_Init(void* data, const int32_t gyro_bias[3], const int32_t accuracy);

/**
* \brief Reset the calibration algorithm state. 
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \warning This method does not reset parameter, bias or accuracy values.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_Reset(void* data);

/**
* \brief Sets parameters with a user-friendly interface.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally. 
* \param[in] algo_mode Configure set of param according to a predefined use case (\sa algo_mode)
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_SetUserParam(void *data, enum algo_mode algo_mode);

/**
* \brief Sets custom parameters.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally. 
* \param[in] duration_us The minimum time [us] required to calibrate gyroscope bias. 
* \param[in] sample_number_log2 Log2 of number of samples to compute the bias, e.g. when sample_number_log2 = 6, number of samples is 2^6=64 samples. Default value is 6.
* \param[in] data_difference_threshold Threshold that detect motion. Restart algorithm when the difference 2 consequetive samples exceed the threshold. Default value is 58. 
* \param[in] high_order_variance_threshold Threshold that detect motion. Restart algorithm when the high order variance exceed the threshold. Default value is 0x28000.
* \param[in] bias_rejection_threshold Threshold that detect motion. Reject bias higher than the threshold. Default value is 40 dps => 40*(2^15/2000)*2^15.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_SetParam(void *data,
								const int32_t		duration_us,
								const int32_t		sample_number_log2,
								const int32_t		data_difference_threshold,
								const int32_t		high_order_variance_threshold,
								const int32_t		bias_rejection_threshold);


/**
* \brief Gets calibration algorithm parameters.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally. 
* \param[in] duration_us The minimum time required to calibrate gyroscope bias. 
* \param[in] sample_number_log2 Log2 of number of samples to compute the bias, e.g. when sample_number_log2 = 6, number of samples is 2^6=64 samples. Default value is 6.
* \param[in] data_difference_threshold Threshold that detect motion. Restart algorithm when the difference 2 consequetive samples exceed the threshold. Default value is 58. 
* \param[in] high_order_variance_threshold Threshold that detect motion. Restart algorithm when the high order variance exceed the threshold. Default value is 0x28000.
* \param[in] bias_rejection_threshold Threshold that detect motion. Reject bias higher than the threshold. Default value is 40 dps => 40*(2^15/2000)*2^15.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_GetParam(void *data,
							int32_t		*duration_us,
							int32_t		*sample_number_log2,
							int32_t		*data_difference_threshold,
							int32_t		*high_order_variance_threshold,
							int32_t		*bias_rejection_threshold);


/**
* \brief Set gyrometer calibration time sampling period.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally. 
* \param[in] sampling_period_us sampling period in us. 
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_SetSamplingPeriod(void *data, const uint32_t sampling_period_us);

/**
* \brief Gets accuracy level of current bias.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \retval 0 Unreliable
* \retval 1 Low
* \retval 2 Medium
* \retval 3 High
* \ingroup GyroCalibration
*/
uint8_t INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_GetAccuracy(void* data);

/**
* \brief Gets calibrated data.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \param[out] gyro_calibrated[3] calibrated data, fxp format is \b 2^30 = 2000dps.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_GetCalibrated(void* data, int32_t gyro_calibrated[3]);

/**
* \brief Gets uncalibrated data.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \param[out] gyro_uncalibrated[3] calibrated data, fxp format is \b 2^30 = 2000dps.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_GetUncalibrated(void* data, int32_t gyro_uncalibrated[3]);

/**
* \brief Sets bias data.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \param[out] gyro_bias[3] gyro bias, fxp format is \b 2^30 = 2000dps.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_SetBias(void *data, const int32_t	bias[3]);

/**
* \brief Sets accuracy level.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \param[out] accuracy gyro bias accuracy level, accuracy values are 0: unreliable, 1: low, 2:medium, 3:High.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_SetAccuracy(void *data, const int32_t	accuracy);

/**
* \brief Gets uncalibrated data.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \param[out] gyro_bias[3] gyro bias, fxp format is \b 2^30 = 2000dps.
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_GetBias(void* data, int32_t gyro_bias[3]);

/**
* \brief Updates the algorithm with new accelerometer sensor data.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_GYR_CAL_FXP_SIZE to use internally.
* \param[in] gyro_raw[3] raw gyroscope data, fxp format is \b 2^15 = 2000dps.
* \param[in] gyro_temperature, gyroscope temperature in °C*100
* \ingroup GyroCalibration
*/
void INV_EXPORT Algo_InvnCalibration_GyroCalibrationFxp_UpdateGyr(void* data, const int32_t gyro_raw[3], const int32_t gyro_temperature);


/* CompassCalibrationGyroOptionalFxp */

/**
* \brief Initializes the calibration algorithm. this function should be called at the first start of sensor and also after a switch off/on of the sensor since the mag offset has potentially changed during off period.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_CCGO_CONFIG_SIZE to use internally. 
* \param[in] offset[3] initial offset [uT]. fxp format is \b q16 \n If pointer is NULL or 0, offset will be set to { 0, 0, 0},
* \param[in] accuracy accuracy of initial offset (0 to 3).
* \param[in] mag_sampling_period sampling period [us] of the magnetometer sensor. If called with mag_sampling_period==0, the previous value of sampling period will be kept.
* \warning This method sets default parameters value, it will resets the algorithm and set accuracy to 0.
* \ingroup CompassCalibrationGyroOptional
*/
void INV_EXPORT Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_Init(void* data, int32_t magOffset[3], int32_t accuracy, uint32_t mag_sampling_period);

/**
* \brief This function allows to change compass sampling period on the fly without reseting the algorithm and so keeping accuracy level.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_CCGO_CONFIG_SIZE to use internally. 
* \param[in] mag_sampling_period sampling period [us] of the magnetometer sensor. If called with mag_sampling_period==0, the previous value of sampling period will be kept.
* \warning This method will keep offset and accuracy level.
* \ingroup CompassCalibrationGyroOptional
*/
void INV_EXPORT Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_SetSamplingPeriod(void* data, uint32_t mag_sampling_period);

/**
* \brief Gets accuracy level of current offset.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_CCGO_CONFIG_SIZE to use internally.
* \retval 0 Unreliable
* \retval 1 Low
* \retval 2 Medium
* \retval 3 High
* \ingroup CompassCalibrationGyroOptional
*/
uint8_t INV_EXPORT Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_GetAccuracy(void* data);

/**
* \brief Gets the norm of the local measured field.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_CCGO_CONFIG_SIZE to use internally.
* \retval field_norm magnitude of local magnetic field [uT] corresponding to the current offset solution. fxp format is \b q16
* \retval 0 if local magnetic field norm can't be computed (no offset computed yet).
* \ingroup CompassCalibrationGyroOptional
*/
uint32_t INV_EXPORT Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_GetFieldNorm(void* data);

/**
* \brief Updates the algorithm with new magnetometer sensor data.
* \param[in] data Address of the structure of size ALGO_INVN_CALIBRATION_CCGO_CONFIG_SIZE to use internally.
* \param[in] mag_uncalib, uncalibrated magnetometer data [uT]. fxp format is \b q16
* \param[out] mag_offset[3] offset [uT], coded in q16.
* \return 1 when new position has been taken into account,
* \return 2 when a new offset has been validated,
* \return 0 otherwise.
* \ingroup CompassCalibrationGyroOptional
*/
int32_t INV_EXPORT Algo_InvnCalibration_CompassCalibrationGyroOptionalFxp_UpdateMag(void* data, const int32_t mag_uncalib[3], int32_t mag_offset[3]);


/*** Orientation ***/

/* BodyToWorldFrameFxp */

/**
* \brief Initializes Orientation algorithm with default parameters and reset states.
* \param[in] data Address of the structure of size ALGO_INVN_ORIENTATION_CONFIG_SIZE to use internally.
* \param[in] product_id: product identification number. \n
	<table>
	<caption id="table_orientation_product_id">Product ID table</caption>
		<tr><td rowspan="1">Mobile	<td> 0
		<tr><td rowspan="1">HMD	(Head Motion Device)	<td> 1
		<tr><td rowspan="1">HHD	(Hand Held Device)		<td> 2
		<tr><td rowspan="1">Drone	<td> 3
	</table>
* \warning Default parameter for sampling period is 10000 us.
* \ingroup BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_Init(void* data, const uint8_t product_id);

/**
* \brief Reset Orientation algorithm states. 
* \param[in] data Address of the structure of size ALGO_INVN_ORIENTATION_CONFIG_SIZE to use internally.
* \ingroup BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_ResetStates(void* data);

/** \brief  Enable orientation output.
	
	\param[in,out] data: Pointer to data structure. 
	\param[in] mask: select one or more orientation to enable. \n
	\t 1: select Geo-mag Rotation Vector (GmRV)
	\t 2: select Game Rotation Vector (GRV)
	\t 4: select Rotation Vector (RV)
	\t 7: to select all 
	
	\ingroup   BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_Enable(void* data, uint8_t mask);

/** \brief  Disable orientation output.
	
	\param[in,out] data: Pointer to data structure. 
	\param[in] mask: select one or more orientation to enable. \n
	\t 1: select Geo-mag Rotation Vector (GmRV)
	\t 2: select Game Rotation Vector (GRV)
	\t 4: select Rotation Vector (RV)
	\t 7: to select all 

	\ingroup   BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_Disable(void* data, uint8_t mask);

/** \brief  Enable Rotation Vector (RV) output.
	
	\param[in,out] data: Pointer to data structure. 
	
	\ingroup   BodyToWorldFrame
*/
static __inline void Algo_InvnOrientation_BodyToWorldFrameFxp_AGM_Enable(void* data)
{
	Algo_InvnOrientation_BodyToWorldFrameFxp_Enable(data, 0x04);
}

/** \brief  Disnable Rotation Vector (RV) output.
	
	\param[in,out] data: Pointer to data structure. 
	
	\ingroup   BodyToWorldFrame
*/
static __inline void Algo_InvnOrientation_BodyToWorldFrameFxp_AGM_Disable(void* data)
{
	Algo_InvnOrientation_BodyToWorldFrameFxp_Disable(data, 0x04);
}

/** \brief  Enable Game Rotation Vector (GRV) output.
	
	\param[in,out] data: Pointer to data structure. 
	
	\ingroup   BodyToWorldFrame
*/
static __inline void Algo_InvnOrientation_BodyToWorldFrameFxp_AG_Enable(void* data)
{
	Algo_InvnOrientation_BodyToWorldFrameFxp_Enable(data, 0x02);
}

/** \brief  Disable Game Rotation Vector (GRV) output.
	
	\param[in,out] data: Pointer to data structure. 
	
	\ingroup   BodyToWorldFrame
*/
static __inline void Algo_InvnOrientation_BodyToWorldFrameFxp_AG_Disable(void* data)
{
	Algo_InvnOrientation_BodyToWorldFrameFxp_Disable(data, 0x02);
}

/** \brief  Enable Geo-mag Rotation Vector (GmRV) output.
	
	\param[in,out] data: Pointer to data structure. 
	
	\ingroup   BodyToWorldFrame
*/
static __inline void Algo_InvnOrientation_BodyToWorldFrameFxp_AM_Enable(void* data)
{
	Algo_InvnOrientation_BodyToWorldFrameFxp_Enable(data, 0x01);
}

/** \brief  Disable Geo-mag Rotation Vector (GmRV) output.
	
	\param[in,out] data: Pointer to data structure. 
	
	\ingroup   BodyToWorldFrame
*/
static __inline void Algo_InvnOrientation_BodyToWorldFrameFxp_AM_Disable(void* data)
{
	Algo_InvnOrientation_BodyToWorldFrameFxp_Disable(data, 0x01);
}

/** \brief  Set accelerometer time stamp and related parameters.	
	\param[in,out] data: Pointer to data structure. 
	\param[in] dt_us: sampling period in micro second.
	\param[in] opt Additional parameters, can be NULL.
	\warning Minimum accelerometer frequency is 10Hz.

    \ingroup BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_SetAccSamplingPeriod(void* data, 
		const int32_t	dt_us,
		void 			*opt);

/** \brief  Set magnetometer custom parameters.	
	\param[in,out] param Pointer to data structure. 
	\param[in] anomaly_rejection anomaly rejection percentage 0 to 100, (anomaly_rejection = percentage * 2^30/100).
	\param[in] thresh_yaw_stop_convergence threshold to stop RV convergence when error is below this threshold, (thresh_yaw_stop_convergence = threshold_rad * 2^30). To disactivate this feature set threshold = -1.
	\param[in] thresh_gyr_stop_convergence threshold to stop RV convergence when gyro is below this threshold, (thresh_gyr_stop_convergence = threshold_dps * 2^30/2000). To disactivate this feature set threshold = -1.
	\param[in] thresh_yaw_smooth_convergence factor [Q16] to smooth RV yaw convergence. The correction speed will be scaled to remain lower than threshold * gyro, (suggested value threshold = 0.01*2^16 = 650). To disactivate this feature set threshold = -1.
	\warning Minimum magnetometer frequency is 10Hz.

    \ingroup BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_SetMagCustomParams(void* data, 
		const int32_t			anomaly_rejection,
		const int32_t			thresh_yaw_stop_convergence,
		const int32_t			thresh_gyr_stop_convergence,
		const int32_t			thresh_yaw_smooth_convergence);

/** \brief Sets parameters with a user-friendly interface.
	\param[in,out] data: Pointer to data structure.
	\param[in] algo_mode Configure set of param according to a predefined use case (\sa algo_mode)

	\ingroup BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_SetMagUserParams(void *data, enum algo_mode algo_mode);

/** \brief  Set magnetometer time stamp and related parameters.	
	\param[in,out] param Pointer to data structure. 
	\param[in] dt_us sampling period in micro second.
	\warning Minimum magnetometer frequency is 10Hz.

    \ingroup BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_SetMagSamplingPeriod(void* data, 
											const int32_t		dt_us);

/** \brief  Set gyroscope time stamp and related parameters.	
	\param[in,out] data: Pointer to data structure. 
	\param[in] dt_us: sampling period in micro second.
	\param[in] opt Additional parameters, can be NULL.
	\warning Minimum gyroscope frequency is 50Hz.

	\ingroup BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_SetGyrSamplingPeriod(void* data, 
											const int32_t		dt_us,
											void				*opt);

/** \brief  Update orientation using accelerometer data.
	
	\param[in,out] data: Pointer to data structure. 
	\param[in] accelerometer: accelerometer data. Units are in [G] fixed point format is \b Q25\b (1G: 33554432). \n. 
	\param[in] acc_accuracy: accelerometer accuracy (accuracy values: 0 Unreliable, 1 Low, 2 Medium, 3 High). \n. 

	\ingroup   BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateAcc(void* data, const int32_t accelerometer[3], const int32_t acc_accuracy);

/** \brief  Update orientation using gyroscope data.
	
	\param[in,out] data: Pointer to data structure. 
	\param[in] gyroscope: gyroscope data. Units are in [dps] fixed point format is \b Q30/2000dps\b (2000 dps: 1073741824). \n. 
	\param[in] gyr_accuracy: gyroscope accuracy (accuracy values: 0 Unreliable, 1 Low, 2 Medium, 3 High). \n. 

	\ingroup   BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateGyr(void* data, const int32_t gyroscope[3], const int32_t gyr_accuracy);

/** \brief  Update orientation using magnetometer data.
	
	\param[in,out] data: Pointer to data structure. 
	\param[in] magnetometer: magnetometer data. Units are in [uT] fixed point format is \b Q16\b \n. 
	\param[in] mag_accuracy: magnetometer accuracy (accuracy values: 0 Unreliable, 1 Low, 2 Medium, 3 High). \n. 
	\param[in] local_mag_field_norm: Norm of local magnetic field. Units are in [uT] fixed point format is \b Q16dps\b \n.

	\ingroup   BodyToWorldFrame
*/
void INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_UpdateMag(void* data, const int32_t magnetometer[3], const int32_t mag_accuracy, const int32_t local_mag_field_norm);

/** \brief  Get game rotation vector (GRV) and accuracy. This orientation uses acclerometer and gyroscope data.
	
	\param[in,out] data: Pointer to data structure. 
	\param[out] quaternion_acc_gyr: quaternion. Fixed point format is \b Q30 \b. \n. 
	\return accuracy: quaternion accuracy (accuracy values: 0 Unreliable, 1 Low, 2 Medium, 3 High). \n. 

	\ingroup   BodyToWorldFrame
*/
int32_t INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_GetGameRotationVector(void* data, int32_t quaternion_acc_gyr[4]);

/** \brief  Get rotation vector (RV) and accuracy. This orientation uses acclerometer, magnetometer and gyroscope data.
	
	\param[in,out] data: Pointer to data structure. 
	\param[out] quaternion_acc_gyr_mag: quaternion. Fixed point format is \b Q30 \b. \n. 
	\return accuracy: quaternion accuracy. Units are in [rad]. Fixed point format is \b Q27 \b (1rad: 134217728). \n. 

	\ingroup   BodyToWorldFrame
*/
int32_t INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_GetRotationVector(void* data, int32_t quaternion_acc_gyr_mag[4]);

/** \brief  Get rotation vector (RV) and accuracy. This orientation uses acclerometer, magnetometer and gyroscope data.
	
	\param[in,out] data: Pointer to data structure. 
	\param[out] quaternion_acc_mag: quaternion. Fixed point format is \b Q30 \b. \n. 
	\return accuracy: quaternion accuracy. Units are in [rad]. Fixed point format is \b Q27 \b (1rad: 134217728). \n. 

	\ingroup   BodyToWorldFrame
*/
int32_t INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_GetGeoMagRotationVector(void* data, int32_t quaternion_acc_mag[4]);

/** \brief  Get gyroscope quaternion integration (LPQ) and accuracy. This orientation uses gyroscope data.
	
	\param[in,out] data: Pointer to data structure. 
	\param[out] quaternion_acc_gyr: quaternion data. Fixed point format is \b Q30 \b. \n. 
	\return accuracy: quaternion accuracy (accuracy values: 0 Unreliable, 1 Low, 2 Medium, 3 High). \n. 

	\ingroup   BodyToWorldFrame
*/
int32_t INV_EXPORT Algo_InvnOrientation_BodyToWorldFrameFxp_GetGyrQuaternion(void* data, int32_t quaternion_gyr[4]);


/* FxpAAR */

/** \defgroup AAR AAR
* \struct Algo_InvnAARInput : input data needed to process AAR algorithm
* \warning proximity, gyrometer and magnetometer sensor are optional, please refers to used configuration. 
* \param acc: calibrated Accelerometers (in fxp s16En14, unit g/2.f) - float to fixpt conversion: fxp = (acc in g * 32768 / 2)
* \param timestampAcc: timestamp of acc sensor (us), used to decimate data during update function
* \param gyr: calibrated gyrometer (in rad/s), can be NULL pointer (if no gyrometer sensor). 
* \param mag: calibrated magnetometer (in fxp s16 format, precision = 0.15uT/bit, native unit uT), can be NULL pointer (if no magnetometer sensor).
* \param Ps: proximity sensor value (5 for not contact, 0 contact with the sensor, can be NULL pointer (if no proximity sensor) 
* \param magNewDataFlag: mag new data is available, equal 1 when a new magnetometer data is available, 0 when no new data is 
* available (if configuration algorithm included magnetometer sensor)
 */
typedef struct
{	int16_t 	acc[3];
    int32_t timestampAcc;
    int16_t 	gyr[3];
    int16_t 	mag[3];
    int16_t Ps;
    uint8_t magNewDataFlag;
} Algo_InvnAARInput;

/** \defgroup AAR AAR
* \struct Algo_InvnAAROutput : output data computed by AAR algorithm
* \param stateBAC :detection BAC event (1:in transport, 2:walking, 4:running, 8:biking, 16:tilting, 32:still)
* (plus any combination of activity, ex: 24=16+8, tilting and biking) \n
* \param statesProb : an integer array containing probabilities (ranging between 0 and 1, and represented in q15) of the different BAC states. 
* The probabilities are output in the following order : still, walking, running, biking, in transport, walking in transport, running in transport
* \param stateSMD : detection SMD event (0: no movement, 1: movement)
* \param statePedometerCounter : detection Counter event()
* \param statePedometerEventStep : detection step event()
* \param statePedometerCadencyPDR : detection Cadency pdr event()
* \param sensorActivateMag: flag indicates when the algorithm needs data from magnetometer sensor (0: do not activate 
* magnetometer sensor, 1: activate magnetometer sensor)
* \param sensorActivateProx: flag indicates when the algorithm needs data from proximity sensor (0: do not activate 
* proximity sensor, 1: activate proximity sensor)
* \param ER1_y1: Enery of Accelerometer low pass filter [1-6.9Hz]
* \param stateTilt: 2-second window average gravity changing by at least 
* 35 degrees since the activation or the last event generated by the sensor (0: no tilt, 1: tilt)
 */
typedef struct
{
    uint8_t stateBAC;
    int32_t stateSMD;
    int32_t statePedometerCounter;
    int8_t 	statePedometerEventStep;
    int32_t statePedometerCadencyPDR;
	uint8_t sensorActivateMag;
	uint8_t sensorActivateProx;
	int32_t ER1_y1;
    uint8_t stateTilt;
} Algo_InvnAAROutput;

/**
* \brief Initializes AAR algorithm with its default values. 
* \param[in] data : structure of size ALGO_INVN_AAR_CONFIG_SIZE
* \ingroup AAR
*/
void INV_EXPORT Algo_InvnAAR_AARFxp_Init(void* data, const int32_t sampling_period);

/**
* \brief Set AAR algorithm parameters. 
* \param[in] data : structure of size INVN_AAR_CONFIG_SIZE
* \param[in] peak_thres: The peak threshold is the absolute value
* of the minimum accelerometer data that is considered a valid step
* (default value 2109832L)
* \param[in] param_min_time: The minimum elapsed time required to constitute a valid step
* (default value 16)
* \param[in] param_max_time: The maximum elapsed time required to constitute a valid step 
* (default value 61)
* \param[in] step_buffer_thres: The minimum number of steps that must be detected before the pedometer step count
* begins incrementing. This is used to prevent false starts of the pedometer when
* the user only takes a small number of steps but stops again quickly. Once the threshold
* is exceeded, the pedometer step count increases by it. The pedometer increments
* regularly thereafter 
* (default value 7)
* \param[in] step_buffer_thres2: The minimum number of low latency steps that must be detected before the pedometer step count
* begins incrementing
* (default value 2)
* \param[in] step_buffer_timer_thres: The pedometer maximum step buffer time. While in the step buffer state, the step buffer
* count resets to 0 if a new step isn’t detected for this amount of time (user is considered
* to have “stopped walking”)
* (default value 150)
* \ingroup AAR
*/	
void INV_EXPORT Algo_InvnAAR_AARFxp_SetParams(void* data,
							int32_t peak_thres, 
							int16_t param_min_time, 
							int16_t param_max_time,  
							int16_t step_buffer_thres , 
							int16_t step_buffer_thres2,  
							int16_t step_buffer_timer_thres);
							
/**
* \brief Reset AAR algorithm with current parameters value. 
* \param[in] data : structure of size INVN_AAR_CONFIG_SIZE
* \ingroup AAR
*/
void INV_EXPORT Algo_InvnAAR_AARFxp_ResetStates(void* data);

/**
* \brief set the working frequency of AAR algorithm
* only 56.25Hz and 50Hz are supported 
* \param[in] data : structure of size INVN_AAR_CONFIG_SIZE
* \param[in] set_period : set value to select algorithm working sampling period, only acceptable value are 17777 for 56.25Hz and 20000 for 50Hz 
* (default value is 17777)
* \ingroup AAR
*/
void INV_EXPORT Algo_InvnAAR_AARFxp_SetSamplingPeriod(void* data, int32_t set_period);

/**
* \brief Performs AAR detection. 
* \param[in] data: structure of size ALGO_INVN_AAR_CONFIG_SIZE
* \param[in] input: structure Algo_InvnAARInput containing input data
* \param[out] output: structure Algo_InvnAAROutput containing output datadetection BAC event (1:in transport, 2:walking, 4:running, 8:biking, 16:tilting, 32:still)
* \return 0 no process data (due to decimation process)
* \return 1 process data (with no problem)
*  \warning internal algorithm nominal accelerometer input frequency is 56.25 Hz or 50Hz (can be set with Algo_InvnAAR_FltAAR_setFrequency function), 
*  i.e., samplingPeriod == 17777us or samplingPeriod == 20000 us. Otherwise, a decimator is used during the update function using timestamp. 
*  Will not work if input frequency is below 56.25Hz / 50Hz. 
* \warning AAR feature can be configured in 5 different ways
* \warning Default, only accelerometer sensor is used (low power mode)
* \warning Mi, INVN_CONFIG_AAR_USE_MI, accelerometer, magnetometer, proximity and SMD vibration mode 
* \warning AM, INVN_CONFIG_AAR_USE_AM, accelerometer, magnetometer sensor 
* \warning APROX, INVN_CONFIG_AAR_USE_APROX, accelerometer, proximity sensor 
* \warning AMPROX, INVN_CONFIG_AAR_USE_AMPROX, accelerometer, magnetometer, proximity sensor 
* \ingroup AAR
*/
uint8_t INV_EXPORT Algo_InvnAAR_AARFxp_Update(void* data,
		Algo_InvnAARInput* input,
		Algo_InvnAAROutput* output);


/* Gesture */
/**
* \brief Initializes Pick_up_L algorithm with default parameters and reset states.
* \param[in] data : structure of size ALGO_INVN_GESTURE_CONFIG_SIZE
* \ingroup Pick_up_L
*/
void INV_EXPORT Algo_InvnGesture_PickUpLFxp_Init(void *data);

/**
* \brief Performs Pick_up_L detection.
* \param[in] data : structure of size ALGO_INVN_GESTURE_CONFIG_SIZE
* \param[in] acc : calibrated accelerometers. Units are in g and axis respect "Movea" axis reference.
* \param[in] proximity : proximity sensor value, has to be 5 if no proximity sensor. 
* \param[out] detection : pick up detection event.
* \return 0.
* \warning Input frequency of accelerometers has to be 56.25Hz, i.e. a sampling period of 17778us.
* \ingroup Pick_up_L
*/
uint8_t INV_EXPORT Algo_InvnGesture_PickUpLFxp_Update(void *data, int16_t acc[3], int16_t proximity, uint8_t *detection);


#endif /* _INV_LIB_ALGO_H_ */

