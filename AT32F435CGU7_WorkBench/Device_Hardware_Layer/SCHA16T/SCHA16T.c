#include "SCHA16T.h"
#include "spi_serve.h"

#include "rthw.h"
#include "at32f435_437_int.h"
#include <stdbool.h>

#if(DT_SCHA1633_ENABLED == 1)

SCH1_filter         Filter;

#define SCHA16T_Transmit_DMA  DMA2_CHANNEL5
#define SCHA16T_Receive_DMA   DMA2_CHANNEL4
#define Device                SPI2

extern rtio_t  rtio_spi2;

SCH1633_Status_Type SCH1633;
uint8_t SCH1633_Initial = 0;

uint32_t sch16T_now_read = 0,first_read = 0;
uint8_t SCH16T_Transmit_Buffer_Asyn[7];
uint8_t SCH16T_Receive_Buffer_Asyn[7];
static void SCHA16T_ReadData_Once(uint64_t cmd,rtio_t *rtio,rtio_node_t *node)
{
		int primask = __get_PRIMASK();
		__set_PRIMASK(1);	
	
		rt_memset(&SCH16T_Transmit_Buffer_Asyn,0,sizeof(SCH16T_Transmit_Buffer_Asyn));
		node->Device_Name = SCH16T_E;
	
		SCH16T_Transmit_Buffer_Asyn[0] = cmd >> 40;
		SCH16T_Transmit_Buffer_Asyn[1] = cmd >> 32;
		SCH16T_Transmit_Buffer_Asyn[2] = cmd >> 24;
		SCH16T_Transmit_Buffer_Asyn[3] = cmd >> 16;
		SCH16T_Transmit_Buffer_Asyn[4] = cmd >> 8; 
		SCH16T_Transmit_Buffer_Asyn[5] = cmd & 0x000000FF;
	
		SCHA16T_CS_enable;
		
		SPI_TransmitReceive_DMA(SCHA16T_Transmit_DMA,\
														SCHA16T_Receive_DMA,\
														(uint32_t)&Device->dt,\
														(uint32_t)&SCH16T_Transmit_Buffer_Asyn,\
														(uint32_t)&SCH16T_Receive_Buffer_Asyn,\
														6
														);
		sch16T_now_read = SCH16T_TEMP;

		__set_PRIMASK(primask);
}

static inline int32_t GetInt32data(uint64_t frame)
{
		return ((int32_t)((frame << 4) & 0xFfFfF000UL)) >> 12;
}

static uint64_t SCH16T_MakeFrame48(const uint8_t *p)
{
    return ((uint64_t)p[0] << 40) |
           ((uint64_t)p[1] << 32) |
           ((uint64_t)p[2] << 24) |
           ((uint64_t)p[3] << 16) |
           ((uint64_t)p[4] << 8)  |
           ((uint64_t)p[5]);
}

static int32_t SCH16T_GetData20(const uint8_t *p)
{
    uint64_t frame = SCH16T_MakeFrame48(p);

    uint32_t raw20 = (uint32_t)((frame >> 8) & 0xFFFFF);

    if (raw20 & 0x80000)
    {
        raw20 |= 0xFFF00000;
    }

    return (int32_t)raw20;
}

static int32_t SCH16T_GetTempRaw(const uint8_t *p)
{
    return SCH16T_GetData20(p) >> 4;
}

SCH16T_t sch16t_out;
uint8_t CRC8(uint64_t SPIframe);

volatile uint32_t drdy_now_us_16t;
volatile uint32_t drdy_last_us_16t;
volatile uint32_t drdy_interval_us_16t;

void interval_us_get()
{
	drdy_now_us_16t =  Micros();
	drdy_interval_us_16t = drdy_now_us_16t - drdy_last_us_16t;
	drdy_last_us_16t = drdy_now_us_16t;
}

void SCHA16T_Conversion(uint8_t *Data_Receive)
{
		int primask = __get_PRIMASK();
	__set_PRIMASK(1);
	
	uint8_t CRCvalue;
	uint64_t frame = SCH16T_MakeFrame48(Data_Receive);
	CRCvalue = CRC8(frame);
	
	uint16_t SA = ((frame>>37) & 0x3FF);
	if(CRCvalue == (uint16_t)(frame & 0xff))
	{
		switch(SA)
		{  
			
			case SCH16T_Acc1_X:{sch16t_out.acc_x = SCH16T_GetData20(Data_Receive)/ACC_TRANS;
												 RawBuffer_Input(&SCH1633.BufAccX,  sch16t_out.acc_x);break;}
			case SCH16T_Acc1_Y:{sch16t_out.acc_y = SCH16T_GetData20(Data_Receive)/ACC_TRANS;
												 RawBuffer_Input(&SCH1633.BufAccY,  sch16t_out.acc_y);break;}
			case SCH16T_Acc1_Z:{sch16t_out.acc_z = SCH16T_GetData20(Data_Receive)/ACC_TRANS;
												 RawBuffer_Input(&SCH1633.BufAccZ,  sch16t_out.acc_z);break;}
			
			case SCH16T_Rate1_X:{sch16t_out.gyro_x = SCH16T_GetData20(Data_Receive)/GYRO_TRANS;
													RawBuffer_Input(&SCH1633.BufGyroX,  sch16t_out.gyro_x);break;}
			case SCH16T_Rate1_Y:{sch16t_out.gyro_y = SCH16T_GetData20(Data_Receive)/GYRO_TRANS;
													RawBuffer_Input(&SCH1633.BufGyroY,  sch16t_out.gyro_y);break;}
			case SCH16T_Rate1_Z:{sch16t_out.gyro_z = SCH16T_GetData20(Data_Receive)/GYRO_TRANS;
													 RawBuffer_Input(&SCH1633.BufGyroZ,  sch16t_out.gyro_z);break;}
			
			case SCH16T_TEMP:{int32_t temp_raw = SCH16T_GetTempRaw(Data_Receive);
												sch16t_out.temp = (float)temp_raw / 100.0f;	
												RawBuffer_Input(&SCH1633.BufTemp,  sch16t_out.temp);
												interval_us_get();break;} 
		}
		if(SA != SCH16T_TEMP)
		{
			SCHA16T_callback(&rtio_spi2);
		}
	}
	else
	{
		__ASM("BKPT #0");
	}
	
	__set_PRIMASK(primask);
}

int SCH1633_Decode(SCH1633_Status_Type* SCH1633)
{
	SCH1633->lastUpdate = Micros(); 
	int primask = __get_PRIMASK();
	__set_PRIMASK(1);
	SCH1633->Accs[0]  = RawBuffer_Output(&SCH1633->BufAccX);
	SCH1633->Accs[1]  = RawBuffer_Output(&SCH1633->BufAccY);
	SCH1633->Accs[2]  = RawBuffer_Output(&SCH1633->BufAccZ)*(-1);
	SCH1633->Gyros[0] = RawBuffer_Output(&SCH1633->BufGyroX)*DEG_TO_RAD;
	SCH1633->Gyros[1] = RawBuffer_Output(&SCH1633->BufGyroY)*(-1)*DEG_TO_RAD;
	SCH1633->Gyros[2] = RawBuffer_Output(&SCH1633->BufGyroZ)*DEG_TO_RAD;
	SCH1633->Temp = RawBuffer_Output(&SCH1633->BufTemp);
	
	if(SCH1633_Initial)
	{
		SCH1633->AccsValid[0] = SCH1633->BufAccX.repeat_count < 50;
		SCH1633->AccsValid[1] = SCH1633->BufAccY.repeat_count < 50;
		SCH1633->AccsValid[2] = SCH1633->BufAccZ.repeat_count < 50;
		SCH1633->GyrosValid[0] = SCH1633->BufGyroX.repeat_count < 50;
		SCH1633->GyrosValid[1] = SCH1633->BufGyroY.repeat_count < 50;
		SCH1633->GyrosValid[2] = SCH1633->BufGyroZ.repeat_count < 50;
		SCH1633->TempValid = SCH1633->BufTemp.repeat_count < 50;
	}
	else
	{
		SCH1633->AccsValid[0] = 0;
		SCH1633->AccsValid[1] = 0;
		SCH1633->AccsValid[2] = 0;
		SCH1633->GyrosValid[0] = 0;
		SCH1633->GyrosValid[1] = 0;
		SCH1633->GyrosValid[2] = 0;
		SCH1633->TempValid = 0;
	}
	__set_PRIMASK(primask);
	return 0;
}

static int SCHA16T_Counter = 0;
void SCHA16T_ReadData(rtio_t *rtio,rtio_node_t *node)
{	
		switch(SCHA16T_Counter){
			default:break;
			case 0:{
				SCHA16T_ReadData_Once(REDE_ACC_Y1 ,rtio ,node);
			}break;
			case 1:{
				SCHA16T_ReadData_Once(REDE_ACC_Z1, rtio ,node);
			}break;
			case 2:{
				SCHA16T_ReadData_Once(REDE_RATE_X1, rtio ,node);
			}break;
			case 3:{
				SCHA16T_ReadData_Once(REDE_RATE_Y1, rtio ,node);
			}break;
			case 4:{
				SCHA16T_ReadData_Once(REDE_RATE_Z1, rtio ,node);
			}break;
			case 5:{
				SCHA16T_ReadData_Once(REDE_TEMP, rtio ,node);
			}break;
			case 6:{
				SCHA16T_ReadData_Once(REDE_ACC_X1, rtio ,node);
				SCHA16T_Counter = -1;
			}break;
		}
		SCHA16T_Counter++;
}


void SCHA16T_callback(rtio_t *rtio)
{
  rtio_node_t *node;
  rt_base_t level;
  rt_bool_t need_wakeup = RT_FALSE;

  node = rtio_acquire(rtio);
  if (node == RT_NULL)
  {
      return;
  }

  node->dev = SCHA16T_ReadData;
  node->op  = 0;

  rtio_submit_sq(rtio, node);

	rt_mb_send(sensor_sqe_mb, (rt_ubase_t)rtio);	
}

bool SCH1_isValidFilterFreq(uint32_t Freq)
{   
    if (Freq == 13 || Freq == 30 || Freq == 68 || Freq == 235 || Freq == 280 || Freq == 370 || Freq == 111) 
        return true;
    else    
        return false;
}

uint32_t SCH1_convertFilterToBitfield(uint32_t Freq)
{
    switch (Freq)
    {
        case 13:
            return 0x092;   // 010 010 010
        case 30:
            return 0x049;   // 001 001 001
        case 68:
            return 0x000;   // 000 000 000        
        case 235:
            return 0x16D;   // 101 101 101
        case 280:
            return 0x0DB;   // 011 011 011
        case 370:
            return 0x124;   // 100 100 100
				case 111:
						return 0x1FF;		// 111 111 111
        default:
            return 0x000;       
    }
}

uint8_t CRC8(uint64_t SPIframe)
{
    uint64_t data = SPIframe & 0xFFFFFFFFFF00LL;
    uint8_t crc = 0xFF;

    for (int i = 47; i >= 0; i--)
    {
        uint8_t data_bit = data >> i & 0x01;
        crc = crc & 0x80 ? (uint8_t)((crc << 1) ^ 0x2F) ^ data_bit : (uint8_t)(crc << 1) | data_bit;
    }
    
    return crc;
}



uint8_t SCH16T_Transmit_Buffer[7];
uint8_t SCH16T_Receive_Buffer[7];

static uint64_t SCHA16T_Read(uint64_t cmd)
{
	uint64_t lower32bit, upper16bit;
	
	SCH16T_Transmit_Buffer[0] = cmd >> 40;
	SCH16T_Transmit_Buffer[1] = cmd >> 32;
	SCH16T_Transmit_Buffer[2] = cmd >> 24;
	SCH16T_Transmit_Buffer[3] = cmd >> 16;
	SCH16T_Transmit_Buffer[4] = cmd >> 8;
	SCH16T_Transmit_Buffer[5] = cmd & 0x000000FF;
	
	SCHA16T_CS_enable;
	
	SPI_TransmitReceive_DMA(SCHA16T_Transmit_DMA,\
													SCHA16T_Receive_DMA,\
													(uint32_t)&Device->dt,\
													(uint32_t)&SCH16T_Transmit_Buffer,\
													(uint32_t)&SCH16T_Receive_Buffer,\
													6
													);
	while (dma_flag_get((uint32_t)SCHA16T_Transmit_DMA) == RESET) {}

	while (dma_flag_get((uint32_t)SCHA16T_Receive_DMA) == RESET) {}
	
	while(SPI2->sts_bit.bf == 1){}
		
	SCHA16T_CS_disable;
		
	volatile uint64_t rx_frame = 0;

	rx_frame |= ((uint64_t)SCH16T_Receive_Buffer[0] << 40);
	rx_frame |= ((uint64_t)SCH16T_Receive_Buffer[1] << 32);
	rx_frame |= ((uint64_t)SCH16T_Receive_Buffer[2] << 24);
	rx_frame |= ((uint64_t)SCH16T_Receive_Buffer[3] << 16);
	rx_frame |= ((uint64_t)SCH16T_Receive_Buffer[4] << 8);
	rx_frame |= ((uint64_t)SCH16T_Receive_Buffer[5]);
			
	return rx_frame;
}

static void SCHA16T_Write(uint64_t cmd)
{
	SCH16T_Transmit_Buffer[0] = cmd >> 40;
	SCH16T_Transmit_Buffer[1] = cmd >> 32;
	SCH16T_Transmit_Buffer[2] = cmd >> 24;
	SCH16T_Transmit_Buffer[3] = cmd >> 16;
	SCH16T_Transmit_Buffer[4] = cmd >> 8;
	SCH16T_Transmit_Buffer[5] = cmd & 0x000000FF;
	
	SCHA16T_CS_enable;
	
	SPI_TransmitReceive_DMA(SCHA16T_Transmit_DMA,\
													SCHA16T_Receive_DMA,\
													(uint32_t)&Device->dt,\
													(uint32_t)&SCH16T_Transmit_Buffer,\
													(uint32_t)&SCH16T_Receive_Buffer,\
													6
													);

	while(SPI2->sts_bit.bf == 1){}
		
	SCHA16T_CS_disable;
}

uint64_t SCHA16T_Packet_Read(uint16_t reg_add)
{
	uint64_t frame = 0;
	uint8_t  CRCvalue = 0;
	frame = (uint64_t)reg_add << 38 | (uint64_t)0 << 37 | (uint64_t)1 << 35;  
	CRCvalue = CRC8(frame);
	frame |= CRCvalue;
	SCHA16T_Read(frame);//0x948000000fa
	uint64_t callback = SCHA16T_Read(frame);//0x4a00000db2a
	CRCvalue = CRC8(callback);
	if(CRCvalue == (uint16_t)(callback & 0xff))return callback;
	else                                       return 0;
}

void SCHA16T_Packet_Write(uint16_t reg_add,uint32_t data)
{
	uint64_t frame = 0;
	uint8_t  CRCvalue = 0;
	frame  = (uint64_t)reg_add << 38 | (uint64_t)1 << 37 | (uint64_t)1 << 35;
	frame |= (data & 0xFFFFF) << 8;
	CRCvalue = CRC8(frame);
	frame |= CRCvalue;	
	SCHA16T_Write(frame);
}

static inline uint16_t getUint16data(uint64_t frame)
{
		return (uint16_t)((frame >> 8) & 0x0000FfFfUL);
}

uint64_t requestFrame_Rate12;
uint64_t responseFrame_Rate12;
uint64_t requestFrame_Acc12;
uint64_t responseFrame_Acc12;
uint64_t requestFrame_Acc3;
uint64_t responseFrame_Acc3;
uint16_t reg_value;
int SCH1_setFilters(uint32_t Freq_Rate12, uint32_t Freq_Acc12, uint32_t Freq_Acc3)
{
    uint32_t dataField;
    uint8_t  CRCvalue;

    if (SCH1_isValidFilterFreq(Freq_Rate12) == false) {
        return SCH1_ERR_INVALID_PARAM;
    }
    if (SCH1_isValidFilterFreq(Freq_Acc12) == false) {
        return SCH1_ERR_INVALID_PARAM;
    }
    if (SCH1_isValidFilterFreq(Freq_Acc3) == false) {
        return SCH1_ERR_INVALID_PARAM;
    }	
		
		
		SCHA16T_Packet_Write(0x25,0x016d);//0x016d
		uint64_t SCH16T_FILE1_Gyro = SCHA16T_Packet_Read(0x25);
		if(getUint16data(SCH16T_FILE1_Gyro) != 0x016d)
		{
			return -1; 
		}
		
		SCHA16T_Packet_Write(0x26,0x016d);
		uint64_t SCH16T_FILE12_ACC12 = SCHA16T_Packet_Read(0x26);
		if(getUint16data(SCH16T_FILE12_ACC12) != 0x016d)
		{
			return -1;
		}
		
		#if(0)
		requestFrame_Rate12 = REQ_SET_FILT_RATE;
    dataField = SCH1_convertFilterToBitfield(235);
		dataField &= 0xFFFFF;
    requestFrame_Rate12 |= dataField;
    requestFrame_Rate12 <<= 8;
    CRCvalue = CRC8(requestFrame_Rate12);
    requestFrame_Rate12 |= CRCvalue;
		SCHA16T_Write(requestFrame_Rate12);//发送 0x9680000db9b 
		
		
		/*尝试读取看配置寄存器是否成功*/
		requestFrame_Rate12  = REQ_SET_FILT_RATE;
		requestFrame_Rate12 &= ~((uint64_t)1 << 29);
		requestFrame_Rate12 <<= 8;
    CRCvalue = CRC8(requestFrame_Rate12);
    requestFrame_Rate12 |= CRCvalue;
		SCHA16T_Read(requestFrame_Rate12);//0x948000000fa
		uint64_t Register = SCHA16T_Read(requestFrame_Rate12);//0x4a00000db2a
		reg_value = SPI48_DATA_UINT16(Register);
		#endif
		
		#if(0)
		requestFrame_Acc12 = REQ_SET_FILT_ACC12;
    dataField = SCH1_convertFilterToBitfield(0);
    requestFrame_Acc12 |= dataField;
    requestFrame_Acc12 <<= 8;
    CRCvalue = CRC8(requestFrame_Acc12);//0x9a80000dbad
    requestFrame_Acc12 |= CRCvalue;
		SCHA16T_Write(requestFrame_Acc12);
		
		/*尝试读取看配置寄存器是否成功*/
		requestFrame_Acc12  = REQ_SET_FILT_ACC12;
		requestFrame_Acc12 &= ~((uint64_t)1 << 29);
		requestFrame_Acc12 <<= 8;
    CRCvalue = CRC8(requestFrame_Acc12);
    requestFrame_Acc12 |= CRCvalue;
		SCHA16T_Read(requestFrame_Acc12);
		Register = SCHA16T_Read(requestFrame_Acc12);//0x4c0000000bc
		reg_value = SPI48_DATA_UINT16(Register);
		#endif
		return 0;
}

int SCH1_Freq_Config()
{
		SCHA16T_Packet_Write(0x28,0x12DB);
		uint64_t SCH16T_Freq_Gyro = SCHA16T_Packet_Read(0x28);
		if(getUint16data(SCH16T_Freq_Gyro) != 0x12DB)
		{
			return -1; 
		}	
		
		SCHA16T_Packet_Write(0x29,0x12DB);
		uint64_t SCH16T_Freq_Acc = SCHA16T_Packet_Read(0x29);
		if(getUint16data(SCH16T_Freq_Acc) != 0x12DB)
		{
			return -1; 
		}	
		return 0;
}

int SCH1_getStatus(SCH1_status *Status)
{
    if (Status == NULL) {
        return SCH1_ERR_NULL_POINTER;
    }
    
    SCHA16T_Read(REQ_READ_STAT_SUM);
    Status->Summary     = getUint16data(SCHA16T_Read(REQ_READ_STAT_SUM_SAT));
    Status->Summary_Sat = getUint16data(SCHA16T_Read(REQ_READ_STAT_COM));
    Status->Common      = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_RATE_COM));
    Status->Rate_Common = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_RATE_X));
    Status->Rate_X      = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_RATE_Y));
    Status->Rate_Y      = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_RATE_Z));
    Status->Rate_Z      = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_ACC_X));    
    Status->Acc_X       = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_ACC_Y));
    Status->Acc_Y       = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_ACC_Z));
    Status->Acc_Z       = SPI48_DATA_UINT16(SCHA16T_Read(REQ_READ_STAT_ACC_Z));

    return SCH1_OK;
}

bool SCH1_verifyStatus(SCH1_status *Status)
{
    if (Status == NULL) {
        return SCH1_ERR_NULL_POINTER;
    }

    if (Status->Summary != 0xffff)
        return false;
    if (Status->Summary_Sat != 0xffff)
        return false;
    if (Status->Common != 0xffff)
        return false;
    if (Status->Rate_Common != 0xffff){
        return false;
		}
    if (Status->Rate_X != 0xffff)
        return false;
    if (Status->Rate_Y != 0xffff)
        return false;
    if (Status->Rate_Z != 0xffff)
        return false;
    if (Status->Acc_X != 0xffff)
        return false;
    if (Status->Acc_Y != 0xffff)
        return false;
    if (Status->Acc_Z != 0xffff)
        return false;
    
    return true;
}

SCH1_status SCH1statusAll;
void SCHA16T_Init()
{
	Filter.Rate12 = FILTER_RATE;
	Filter.Acc12  = FILTER_ACC12;
	Filter.Acc3   = FILTER_ACC3;
	
	int ret = SCH1_OK;
	uint8_t startup_attempt = 0;
	bool SCH1status = false;
	uint16_t comp_id = (uint16_t)((SCHA16T_Packet_Read(0x3c)>> 8) & 0xFFFF);
	
	if(comp_id != SCH1633_Comp_ID)return;
	
	for (startup_attempt = 0; startup_attempt < 2; startup_attempt++) 
	{
		rt_thread_delay(32);
		
		SCH1_setFilters(Filter.Rate12, Filter.Acc12, Filter.Acc3);
		SCH1_Freq_Config();
		SCHA16T_Write(SCHA_16T_EN_SENSOR);
		rt_thread_delay(215);
		SCH1_getStatus(&SCH1statusAll);	
		SCHA16T_Write(SCHA_16T_EN_EOI);			// 启动传感器
		
		rt_thread_delay(3);												// 按照手册等待3ms
		
		SCH1_getStatus(&SCH1statusAll);			// 读取传感器所有状态标志位 两次
		SCH1_getStatus(&SCH1statusAll);
		
		if (!SCH1_verifyStatus(&SCH1statusAll)) 
		{		
				SCH1status = false;            
		}
		else 
		{
				SCH1status = true;     					// 如果传感器没有问题，则跳出循环      
				break;
		}
		
		SCH1_getStatus(&SCH1statusAll);			// 读取传感器所有状态标志位 两次
		SCH1_getStatus(&SCH1statusAll);
		
		if (!SCH1_verifyStatus(&SCH1statusAll)) 
		{		// 检查所有状态位
				SCH1status = false;            
		}
		else 
		{
				SCH1status = true;     					// 如果传感器没有问题，则跳出循环      
				break;
		}
	}
	
	if (SCH1status != true)					// 如果传感器数据有问题，则返回初始化失败 
			ret = SCH1_ERR_SENSOR_INIT;
	else 
		SCH1633_Initial = 1;
}
#endif
