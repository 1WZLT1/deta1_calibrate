#ifndef __icm42688_h
#define __icm42688_h

#include "devicetree_generated.h"
#if (DT_IIM42652_ENABLED == 1)
#include "spi_serve.h"

#include "Icm426xxDriver_HL.h"
#include "sensor.h"

#include "at32f435_437_int.h"
#include "at32f435_437_gpio.h"
#include "rtthread.h"

#include "stdint.h"
#include "rtio.h"

#define SERIF_TYPE ICM426XX_UI_SPI4
#define IS_LOW_NOISE_MODE 1
#define TMST_CLKIN_32K 1

#define ICM426XX_AccScale  (9.7947f * 8.0f / 32768.0f)
#define ICM426XX_GyroScale ((3.1415926f * 2000.0f / 180.0f) / 32768.0f)
//#define ICM426XX_GyroScale (2000.0f / 32768.0f)

#define ICM42688_CS_enable            gpio_bits_write(GPIOA,GPIO_PINS_15,FALSE);
#define ICM42688_CS_disenable         gpio_bits_write(GPIOA,GPIO_PINS_15,TRUE);

typedef struct
{
	RawBuffer_t				          BufGyroX;
	RawBuffer_t				          BufGyroY;
	RawBuffer_t				          BufGyroZ;
	RawBuffer_t				          BufAccX;
	RawBuffer_t				          BufAccY;
	RawBuffer_t				          BufAccZ;
	RawBuffer_t				          BufTemp;
	
	float                       Temp;
	float                       Accs[3];
	float                       Gyros[3];
	uint8_t                     AccsValid[3];
	uint8_t                     GyrosValid[3];
	uint8_t                     TempValid;
	
	volatile uint64_t           lastUpdate;
}ICM42688_Status_Type;

typedef struct
{
	float                       Temp;
	float                       Accs[3];
	float                       Gyros[3];
}ICM42688_BufferData_Type;

enum ICM42688_ACC_RAGE
{
	ICM42688_ACC_RANGE_8g,
	ICM42688_ACC_RANGE_2g,
	ICM42688_ACC_RANGE_4g,
	ICM42688_ACC_RANGE_16g,
};

enum ICM42688_GYO_RAGE
{	
	ICM42688_GYO_RANGE_2000dps,  /*!< 2000dps*/
	ICM42688_GYO_RANGE_16dps  ,  /*!< 16dps*/
	ICM42688_GYO_RANGE_31dps  ,  /*!< 31dps*/
	ICM42688_GYO_RANGE_62dps  ,  /*!< 62dps*/
	ICM42688_GYO_RANGE_125dps ,  /*!< 125dps*/
	ICM42688_GYO_RANGE_250dps ,  /*!< 250dps*/
	ICM42688_GYO_RANGE_500dps ,  /*!< 500dps*/
	ICM42688_GYO_RANGE_1000dps,  /*!< 1000dps*/
};

void ICM42688_callback(rtio_t *rtio);
void ICM42688_Conversion(uint8_t *Data_Receive);
void ICM42652_Read_Reg(uint8_t  reg,uint8_t  *data,uint16_t len);
void ICM42688_Write_Reg(uint8_t  reg, uint8_t  data,uint16_t len);
void ICM42652_Read_INT1_Config(void);
void ICM42652_CLKIN_Config(void);

void gpio_toggle(gpio_type *gpio_x, uint16_t pins);

int inv_io_hal_read_reg(struct inv_icm426xx_serif *serif, uint8_t reg, uint8_t *buffer, uint32_t len);
int inv_icm426xx_read_reg(struct inv_icm426xx * s, uint8_t reg, uint32_t len, uint8_t * buf);
int iotex_icm42605_init(void);
void icm42688_number_get(void);
void icm42688_get(uint8_t regi,uint32_t reserve_address);
float ICM42688_DataToFloat(uint8_t* p);
int ICM42688_Decode(ICM42688_Status_Type* ICM42688);

extern struct rt_semaphore ICM42688_Semaphore;
extern uint8_t Icm42688_Init_Over;
extern uint8_t icm42688_buffer[28];
extern uint8_t icm42688_data_read_register[2];
extern uint8_t icm42688_data_reiseive_fun_switch;
extern ICM42688_Status_Type ICM42688;

//uint8_t ICM42688_Data_Transmit[15];
extern uint8_t ICM42688_Data_Receive[30];
extern ICM42688_BufferData_Type ICM42688_BufferData;
//extern static uint8_t whoamI1;
#endif


#endif

