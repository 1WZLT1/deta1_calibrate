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
#ifndef _ALGO_EAPI_H_
#define _ALGO_EAPI_H_
 
#include <stdint.h>

#define DEFAULT_ODR_US   20000

/* These need to be defined by the application */
extern int32_t cfg_acc_fsr;
extern int32_t cfg_gyr_fsr;

/*!
 * \brief Algorithm inputs (raw data)
 */
typedef struct algo_input {
	int32_t sRacc_data[3];           ///< raw accel
	int32_t sRgyro_data[3];          ///< raw gyro
	int16_t sRtemp_data;             ///< raw temp
	int32_t sRmag_data[3];           ///< raw mag
} algo_input;


/*!
 * \brief Algorithm outputs (calibrated, bias)
 */
typedef struct algo_output {
	int32_t acc_cal_q16[3];          ///< calibrated accel
	int32_t acc_bias_q16[3];         ///< accel bias
	int8_t  acc_accuracy_flag;       ///< accel accuracy

	int32_t gyr_cal_q16[3];          ///< calibrated gyro
	int32_t gyr_uncal_q16[3];        ///< uncalibrated gyro
	int32_t gyr_bias_q16[3];         ///< gyro bias
	int8_t  gyr_accuracy_flag;       ///< gyro accuracy

	int32_t grv_quat_q30[4];         ///< 6-axis (a/g) quaternion
	int32_t gravity_q16[3];
	int32_t linearacc_q16[3];

	int32_t mag_cal_q16[3];          ///< calibrated mag
	int32_t mag_uncal_q16[3];        ///< uncalibrated mag
	int32_t mag_bias_q16[3];         ///< mag bias
	int8_t  mag_accuracy_flag;       ///< mag accuracy

	int32_t rv_quat_q30[4];          ///< 9-axis quaternion
	int32_t rv_accuracy;             ///< 9-axis quaternion accuracy

	int32_t temp_degC_q16;           ///< temperature

	float euler_angles[3];           ///< euler angles
	} algo_output;

/*!
 */
typedef enum algo_params
{
	ALGO_PARAM_MAX
} algo_params;

/* algo init parameters structure */
typedef struct algo_params_init {
	int32_t * acc_bias_q16;    ///<  acc_bias_q16 Previously stored accel bias
	int32_t * gyr_bias_q16;    ///<  gyr_bias_q16 Previously stored gyro bias
	int32_t * mag_bias_q16;    ///<  mag_bias_q16 Previously stored mag bias (NULL if mag is not present)

	int32_t acc_fsr;           ///<  accelerometer FSR (required unit is mg) 
	int32_t gyr_fsr;           ///<  gyroscope FSR (required unit is dps)

	uint8_t is_high_res_en;    ///<  is fifo high resolution enable
        
        int32_t temp_sensitivity;  ///<  temperature sensitivity (required unit is (LSB/°C)*100 )
        int32_t temp_offset;       ///<  temperature offset (required unit is °C)
	void *params;              ///<  Chip description
	
} algo_params_init;

/*!
 * \brief Initializes algorithms with default parameters and reset states.
 * \param[in] algo init parameters structure
 * \return HMD capabilities of the device
 * \retval 0 device does not support HMD/VR specific algorithms.
 * \retval 1 device supports HMD/VR specific algorithms.
 */
uint8_t algorithms_init(algo_params_init * algo_parameters);


/*!
 * \brief Sets ODR parameter.
 * \param[in] odr_us accel and gyro odr in us.
 * \param[in] mag_odr_us mag odr in us (0 if mag is not present).
 */
void algorithms_configure_odr(uint32_t odr_us, uint32_t mag_odr_us);


/*!
 * \brief Enables/Disables algorithm output.
 * \param[in] enable: 1: enable; 0: disable.
 */
void algorithms_sensor_control(uint32_t enable);


/*!
 * \brief Performs algorithm computation.
 * \param[in] inputs sensor raw data input.
 * \param[out] outputs algorithm output.
 */
void algorithms_process(algo_input *inputs, algo_output *outputs);

#endif /* !_ALGO_EAPI_H_ */
