#include "xv7001.h"

#if (DT_XV7011_ENABLED == 1)
#include "spi_serve.h"
#include "sensor.h"

#include "rthw.h"
#include "at32f435_437_gpio.h"
#include "at32f435_437_int.h"

#define xv7001_Transmit_DMA  DMA2_CHANNEL5
#define xv7001_Receive_DMA   DMA2_CHANNEL4
#define Device                SPI2

#define XV7011_GYRO_Scale_factor_24bit (17920.0f)
#define GyroScale (-1.0f * 3.1415926f / 180.0f / XV7011_GYRO_Scale_factor_24bit / 4.0f)

static float XV7011_GyroDataToFloat(uint8_t *p)
{
    uint32_t rawdata = p[1] << 16 | p[2] << 8 | p[3];
    int32_t v = rawdata << 8; //align to msb
    v >>= 8;
    return (float)v;
}

float xv7001_x_gyro_raw;
XV7011_Status_Type XV7011;
void XV7001_Conversion(uint8_t *Data_Receive)
{
	xv7001_x_gyro_raw = XV7011_GyroDataToFloat(&Data_Receive[0]) * GyroScale;
	RawBuffer_Input(&XV7011.BufGyro,  xv7001_x_gyro_raw);
}

int XV7011_Decode(XV7011_Status_Type *XV7011)
{
    XV7011->lastUpdate = Micros();
    int primask = __get_PRIMASK();
    __set_PRIMASK(1);
    XV7011->Gyro_xv7011 = RawBuffer_Output(&XV7011->BufGyro);

    XV7011->GyroValid = XV7011->BufGyro.repeat_count < 50;
    __set_PRIMASK(primask);
    return 0;
}

uint8_t XV7001_Data_Transmit[4];
uint8_t XV7001_Data_Recive[4];
void XV7001_ReadData(rtio_t *rtio,rtio_node_t *node)
{
		int primask = __get_PRIMASK();
		__set_PRIMASK(1);
		node->Device_Name = XV7001_E;
	
		XV7001_Data_Transmit[0] = 0x0A | 0x80 ;
		xv7001_CS_enable;
	
		SPI_TransmitReceive_DMA(xv7001_Transmit_DMA,\
														xv7001_Receive_DMA,\
														(uint32_t)&Device->dt,\
														(uint32_t)&XV7001_Data_Transmit,\
														(uint32_t)&XV7001_Data_Recive,\
														4
														);
	
	 __set_PRIMASK(primask);	
}

void xv7001_callback(rtio_t *rtio)
{
  rtio_node_t *node;

  node = rtio_acquire(rtio);
  if (node == RT_NULL)
  {
      return;
  }

  node->dev = XV7001_ReadData;
  node->op  = 0;

  rtio_submit_sq(rtio, node);

	rt_mb_send(sensor_sqe_mb, (rt_ubase_t)rtio);	
}

uint8_t xv1633_Transmit_Buffer[2];
uint8_t xv1633_Receive_Buffer[2];

static void xv7001_Write_Reg(uint8_t  reg,\
																 uint8_t  data,\
																 uint16_t len)
{
	xv1633_Transmit_Buffer[0] = reg & 0x7f;
	xv1633_Transmit_Buffer[1] = data;
	
	xv7001_CS_enable;
	
	SPI_TransmitReceive_DMA(xv7001_Transmit_DMA,\
													xv7001_Receive_DMA,\
													(uint32_t)&Device->dt,\
													(uint32_t)&xv1633_Transmit_Buffer,\
													(uint32_t)&xv1633_Receive_Buffer,\
													len
													);

	while(Device->sts_bit.bf == 1){}
		
	xv7001_CS_disable;
}

void xv7001_Read_Reg(uint8_t  reg,\
										 uint8_t  *data,\
										 uint16_t len)
{
		xv1633_Transmit_Buffer[0] = reg | 0x80;
		xv1633_Transmit_Buffer[1] = 0x00;
	
		xv7001_CS_enable;
		SPI_TransmitReceive_DMA(xv7001_Transmit_DMA,\
														xv7001_Receive_DMA,\
														(uint32_t)&Device->dt,\
														(uint32_t)&xv1633_Transmit_Buffer,\
														(uint32_t)&xv1633_Receive_Buffer,\
														len
														);
		while(Device->sts_bit.bf == 1){}
		xv7001_CS_disable;
			
		*data = xv1633_Receive_Buffer[1];
}	

void Filter_Config()
{
	xv7001_Write_Reg(0x01,0x01,2);
	xv7001_Write_Reg(0x02,0x0D,2);
	
}

void Freq_Config()
{
	xv7001_Write_Reg(0x03,0x01,2);
}

void Data_Width()
{
	xv7001_Write_Reg(0x0b,0x05,2);
	
	uint8_t reg;
	xv7001_Read_Reg(0x1c,&reg,2);
	reg &= (uint8_t)~((1U << 6) | (1U << 5));
	xv7001_Write_Reg(0x1c,reg,2);
}


void xv7001_Init()
{
	rt_thread_delay(200);
	xv7001_Write_Reg(0x07,0x00,1);
	rt_thread_delay(8);
	Filter_Config();
	Freq_Config();
	Data_Width();
	
	/* 退出待机，并复位DSP */
  xv7001_Write_Reg(0x06,0x00,1);
  rt_thread_delay(10);
}
#endif
