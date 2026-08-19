#ifndef __SCHA16T_H
#define __SCHA16T_H

#include "stdint.h"
#include "sensor.h"

#include "rtio.h"

#define SCHA16T_CS_enable     gpio_bits_write(GPIOA,GPIO_PINS_11,FALSE);    
#define SCHA16T_CS_disable    gpio_bits_write(GPIOA,GPIO_PINS_11,TRUE);

#define SCH1_OK                      0  // 操作成功，返回码为 0，表示一切正常
#define SCH1_ERR_NULL_POINTER       -1  // 空指针错误，返回码为 -1，表示函数参数中传入了空指针
#define SCH1_ERR_INVALID_PARAM      -2  // 参数无效错误，返回码为 -2，表示函数参数不符合要求
#define SCH1_ERR_SENSOR_INIT        -3  // 传感器初始化错误，返回码为 -3，表示传感器初始化失败
#define SCH1_ERR_OTHER              -4  // 其他错误，返回码为 -4，表示发生了其他未知错误

// Status
// 定义了读取不同状态寄存器的请求命令
#define REQ_READ_STAT_SUM           0x05080000001C  // 读取汇总状态（Summary Status）寄存器的请求命令
#define REQ_READ_STAT_SUM_SAT       0x0548000000EB  // 读取汇总状态饱和寄存器（Summary Saturation Status）的请求命令
#define REQ_READ_STAT_COM           0x0588000000DD  // 读取通用状态（Common Status）寄存器的请求命令
#define REQ_READ_STAT_RATE_COM      0x05C80000002A  // 读取角速度通用状态（Rate Common Status）寄存器的请求命令
#define REQ_READ_STAT_RATE_X        0x0608000000C4  // 读取 X 轴角速度状态（Rate X Status）寄存器的请求命令
#define REQ_READ_STAT_RATE_Y        0x064800000033  // 读取 Y 轴角速度状态（Rate Y Status）寄存器的请求命令
#define REQ_READ_STAT_RATE_Z        0x068800000005  // 读取 Z 轴角速度状态（Rate Z Status）寄存器的请求命令
#define REQ_READ_STAT_ACC_X         0x06C8000000F2  // 读取 X 轴加速度状态（Acceleration X Status）寄存器的请求命令
#define REQ_READ_STAT_ACC_Y         0x070800000069  // 读取 Y 轴加速度状态（Acceleration Y Status）寄存器的请求命令
#define REQ_READ_STAT_ACC_Z         0x07480000009E  // 读取 Z 轴加速度状态（Acceleration Z Status）寄存器的请求命令

#define FILTER_RATE         280.0f       // Hz, LPF1 Nominal Cut-off Frequency (-3dB).
#define FILTER_ACC12        280.0f
#define FILTER_ACC3         280.0f


#define FILTER_LPF 0x096800009205  
 
/**
 * Frame field masks
 */
#define TA_FIELD_MASK               0xFFC000000000   // 目标地址（TA）字段掩码
#define SA_FIELD_MASK               0x7FE000000000   // 源地址（SA）字段掩码
#define DATA_FIELD_MASK             0x00000FFFFF00   // 数据字段掩码
#define CRC_FIELD_MASK              0x0000000000FF   // 循环冗余校验（CRC）字段掩码
#define ERROR_FIELD_MASK            0x001E00000000   // 错误状态字段掩码


// Filters
#define REQ_READ_FILT_RATE          0x0948000000FA
#define REQ_READ_FILT_ACC12         0x0988000000CC
#define REQ_READ_FILT_ACC3          0x09C80000003B
#define REQ_READ_RATE_CTRL          0x0A08000000D5
#define REQ_READ_ACC12_CTRL         0x0A4800000022
#define REQ_READ_ACC3_CTRL          0x0A8800000014
#define REQ_READ_MODE_CTRL          0x0D4800000010
#define REQ_SET_FILT_RATE           0x0968000000    // For building Rate_XYZ1/2 filter setting frame.
#define REQ_SET_FILT_ACC12          0x09A8000000    // For building Acc_XYZ1/2 filter setting frame.
#define REQ_SET_FILT_ACC3           0x09E8000000    // For building Acc_XYZ3 filter setting frame.



#define SPI48_DATA_INT32(a)         (((int32_t)(((a) << 4)  & 0xfffff000UL)) >> 12)
#define SPI48_DATA_UINT32(a)        ((uint32_t)(((a) >> 8)  & 0x000fffffUL))
#define SPI48_DATA_UINT16(a)        ((uint16_t)(((a) >> 8)  & 0x0000ffffUL))
#define GET_TEMPERATURE(a)          ((a) / 100.0f)


#define TMEP_TRANS 										100.f
#define ACC_TRANS  										3200.0f
#define GYRO_TRANS 										1600.0f

#define SCHA_16T_EN_SENSOR 						0x0D68000001D3
#define SCHA_16T_EN_EOI 	 						0x0D680000038D

#define REDE_ACC_X1  									0x0108000000F6
#define REDE_ACC_Y1  									0x014800000001
#define REDE_ACC_Z1  									0x018800000037
#define REDE_RATE_X1 									0x0048000000AC
#define REDE_RATE_Y1 									0x00880000009A
#define REDE_RATE_Z1 									0x00C80000006D
#define REDE_TEMP		 									0x0408000000B1

#define SCHA16T_ID_ACCX  	    				0
#define SCHA16T_ID_ACCY  	    				1
#define SCHA16T_ID_ACCZ  	    				2
#define SCHA16T_ID_GYROX      				3
#define SCHA16T_ID_GYROY 	    				4
#define SCHA16T_ID_GYROZ 		  				5
#define SCHA16T_ID_TEMP  							6

typedef struct _SCH1_filter {
    uint16_t Rate12;
    uint16_t Acc12;
    uint16_t Acc3;
} SCH1_filter;

typedef struct SCHA16T_Type
{
	uint64_t lastcmd;
	uint8_t lasttype;

	RawBuffer_t Accx_Buf;						// 用于每个轴的数据平均
	RawBuffer_t Accy_Buf;
	RawBuffer_t Accz_Buf;
	RawBuffer_t Gxro_Buf;
	RawBuffer_t Gyro_Buf;
	RawBuffer_t Gzro_Buf;
	RawBuffer_t tempdue_Buf;
	RawBuffer_t tempuno_Buf;	
	
	float 	Accs[3];								// 存储最总数据
	float		Gyros[3];
	uint8_t AccsValid[3];						// 数据是否有效标志位
	uint8_t	GyrosValid[3];
	uint8_t tempValid[2];
	
	float		temp_uno;
	
	volatile uint8_t Initialized;		// 初始化标志位
	volatile uint64_t Lastupdate;
	struct rt_semaphore Semaphore;
}SCHA16T_Status_Type;

typedef struct _SCH1_status {
    uint16_t Summary;       // 状态总结，可能是设备整体状态的汇总信息
    uint16_t Summary_Sat;   // 状态卫星，可能与卫星相关的数据或状态
    uint16_t Common;        // 通用状态，存储设备的常规状态信息
    uint16_t Rate_Common;   // 速率通用状态，可能与旋转速率或运动状态相关
    uint16_t Rate_X;        // X轴的速率状态（可能是角速率或线性速率）
    uint16_t Rate_Y;        // Y轴的速率状态（可能是角速率或线性速率）
    uint16_t Rate_Z;        // Z轴的速率状态（可能是角速率或线性速率）
    uint16_t Acc_X;         // X轴加速度状态
    uint16_t Acc_Y;         // Y轴加速度状态
    uint16_t Acc_Z;         // Z轴加速度状态
} SCH1_status;

typedef struct {
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float temp;
} SCH16T_t;

enum
{
    SCH16T_Rate1_X = 0x0001,
    SCH16T_Rate1_Y = 0x0002,
    SCH16T_Rate1_Z = 0x0003,

    SCH16T_Acc1_X  = 0x0004,
    SCH16T_Acc1_Y  = 0x0005,
    SCH16T_Acc1_Z  = 0x0006,

    SCH16T_TEMP    = 0x0010,
};

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
}SCH1633_Status_Type;

#define SCH1633_Comp_ID 0x002E

extern uint32_t sch16T_now_read;
extern SCH1633_Status_Type SCH1633;

extern uint8_t SCH16T_Transmit_Buffer_Asyn[7];
extern uint8_t SCH16T_Receive_Buffer_Asyn[7];
extern SCH16T_t sch16t_out;

void SCHA16T_Init(void);
void SCHA16T_callback(rtio_t *rtio);
void SCHA16T_Conversion(uint8_t *Data_Receive);
int SCH1633_Decode(SCH1633_Status_Type* SCH1633);
static uint64_t SCH16T_MakeFrame48(const uint8_t *p);

#endif
