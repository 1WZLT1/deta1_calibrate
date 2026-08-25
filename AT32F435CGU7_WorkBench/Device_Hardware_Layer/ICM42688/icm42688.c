#include "icm42688.h"
#include "rtio.h"
#include "math.h"

#if (DT_IIM42652_ENABLED == 1)
#define ICM42688_Transmit_DMA			DMA1_CHANNEL6
#define ICM42688_Receive_DMA 			DMA1_CHANNEL5
#define Device    								SPI1

uint8_t ICM42688_Data_Transmit[30];
uint8_t ICM42688_Data_Receive[30];
static uint8_t whoamI1;

static struct inv_icm426xx icm_driver;

void inv_icm426xx_sleep_us(uint32_t us)
{
	rt_thread_delay((us + 999) / 1000);
}

void ICM42688_transmit(rtio_t *rtio,rtio_node_t *node)
{
		int primask = __get_PRIMASK();
		__set_PRIMASK(1);
	
		rt_memset(&ICM42688_Data_Transmit,0,sizeof(ICM42688_Data_Transmit));
		rt_memset(ICM42688_Data_Receive,  0, sizeof(ICM42688_Data_Receive));
	
		node->Device_Name = ICM42688_E;
	
		ICM42688_Data_Transmit[0] = 0x1D | 0x80;
		ICM42688_CS_enable;
		
		SPI_TransmitReceive_DMA(ICM42688_Transmit_DMA,\
														ICM42688_Receive_DMA,\
														(uint32_t)&Device->dt,\
														(uint32_t)&ICM42688_Data_Transmit,\
														(uint32_t)&ICM42688_Data_Receive,\
														15
														);
	 __set_PRIMASK(primask);
}

static inline int16_t ICM42688_GetS16(const uint8_t *p)
{
    return (int16_t)(((uint16_t)p[0] << 8) | p[1]);
}

volatile uint32_t counts= 0;
void ICM42688_callback(rtio_t *rtio)
{
  rtio_node_t *node;
  rt_base_t level;
  rt_bool_t need_wakeup = RT_FALSE;

  node = rtio_acquire(rtio);
  if (node == RT_NULL)
  {
      return;
  }

  node->dev = ICM42688_transmit;
  node->op  = 0;

  rtio_submit_sq(rtio, node);

	rt_mb_send(sensor_sqe_mb, (rt_ubase_t)rtio);
	
	counts++;
}

int ICM42688_Decode(ICM42688_Status_Type* ICM42688)
{
	ICM42688->lastUpdate = Micros(); 
	int primask = __get_PRIMASK();
	__set_PRIMASK(1);
	ICM42688->Accs[0]  = RawBuffer_Output(&ICM42688->BufAccX);
	ICM42688->Accs[1]  = RawBuffer_Output(&ICM42688->BufAccY);
	ICM42688->Accs[2]  = RawBuffer_Output(&ICM42688->BufAccZ);
	ICM42688->Gyros[0] = RawBuffer_Output(&ICM42688->BufGyroX);
	ICM42688->Gyros[1] = RawBuffer_Output(&ICM42688->BufGyroY);
	ICM42688->Gyros[2] = RawBuffer_Output(&ICM42688->BufGyroZ);
	ICM42688->Temp = RawBuffer_Output(&ICM42688->BufTemp);
	
	if(Icm42688_Init_Over)
	{
		ICM42688->AccsValid[0] = ICM42688->BufAccX.repeat_count < 50;
		ICM42688->AccsValid[1] = ICM42688->BufAccY.repeat_count < 50;
		ICM42688->AccsValid[2] = ICM42688->BufAccZ.repeat_count < 50;
		ICM42688->GyrosValid[0] = ICM42688->BufGyroX.repeat_count < 50;
		ICM42688->GyrosValid[1] = ICM42688->BufGyroY.repeat_count < 50;
		ICM42688->GyrosValid[2] = ICM42688->BufGyroZ.repeat_count < 50;
		ICM42688->TempValid = ICM42688->BufTemp.repeat_count < 50;
	}
	else
	{
		ICM42688->AccsValid[0] = 0;
		ICM42688->AccsValid[1] = 0;
		ICM42688->AccsValid[2] = 0;
		ICM42688->GyrosValid[0] = 0;
		ICM42688->GyrosValid[1] = 0;
		ICM42688->GyrosValid[2] = 0;
		ICM42688->TempValid = 0;
	}
	__set_PRIMASK(primask);
	return 0;
}

uint64_t last_us1 = 0,now_us1 = 0,Interval_us1 = 0;
float  acc_test = 0;
float  gyro_test = 0;
ICM42688_BufferData_Type ICM42688_BufferData;
void ICM42688_Conversion(uint8_t *Data_Receive)//ICM42688_Data_Receive
{
  int primask = __get_PRIMASK();
    __set_PRIMASK(1);

    now_us1 = Micros();
    Interval_us1 = now_us1 - last_us1;

    ICM42688_BufferData.Temp = 25.0f + (ICM42688_GetS16(&Data_Receive[0]) / 132.48f);

    ICM42688_BufferData.Accs[0] = ICM426XX_AccScale  * ICM42688_GetS16(&Data_Receive[2]);
    ICM42688_BufferData.Accs[1] = ICM426XX_AccScale  * ICM42688_GetS16(&Data_Receive[4]);
    ICM42688_BufferData.Accs[2] = ICM426XX_AccScale  * ICM42688_GetS16(&Data_Receive[6]);

    ICM42688_BufferData.Gyros[0] = ICM426XX_GyroScale * ICM42688_GetS16(&Data_Receive[8]);
    ICM42688_BufferData.Gyros[1] = ICM426XX_GyroScale * ICM42688_GetS16(&Data_Receive[10]);
    ICM42688_BufferData.Gyros[2] = ICM426XX_GyroScale * ICM42688_GetS16(&Data_Receive[12]);
	
	
    RawBuffer_Input(&ICM42688.BufAccX,  ICM42688_BufferData.Accs[0]);
    RawBuffer_Input(&ICM42688.BufAccY,  ICM42688_BufferData.Accs[1]);
    RawBuffer_Input(&ICM42688.BufAccZ,  ICM42688_BufferData.Accs[2]);

    RawBuffer_Input(&ICM42688.BufGyroX, ICM42688_BufferData.Gyros[0]);
    RawBuffer_Input(&ICM42688.BufGyroY, ICM42688_BufferData.Gyros[1]);
    RawBuffer_Input(&ICM42688.BufGyroZ, ICM42688_BufferData.Gyros[2]);
		
    RawBuffer_Input(&ICM42688.BufTemp,  ICM42688_BufferData.Temp);
		
    last_us1 = now_us1;

    __set_PRIMASK(primask);
}

//寄存器配置
int ConfigureInvDevice(uint8_t is_low_noise_mode,
                       ICM426XX_ACCEL_CONFIG0_FS_SEL_t acc_fsr_g,
                       ICM426XX_GYRO_CONFIG0_FS_SEL_t gyr_fsr_dps,
                       ICM426XX_ACCEL_CONFIG0_ODR_t acc_freq,
                       ICM426XX_GYRO_CONFIG0_ODR_t gyr_freq,
                       uint8_t is_rtc_mode)
{
	int rc = 0;
	
	rc |= inv_icm426xx_enable_clkin_rtc(&icm_driver, is_rtc_mode);//设置int2成外部时钟输入 | 使能时间戳 | 设置如果有Pll就用Pll 否则用内部振荡器 需要rtc时钟输入
	
	rc |= inv_icm426xx_set_accel_fsr(&icm_driver, acc_fsr_g);//加表1k输出 8g
	
	rc |= inv_icm426xx_set_gyro_fsr(&icm_driver, gyr_fsr_dps);//陀螺1k ±2000dps
	
	rc |= inv_icm426xx_set_accel_frequency(&icm_driver, acc_freq);//加表1k 8g
	
	rc |= inv_icm426xx_set_gyro_frequency(&icm_driver, gyr_freq);//陀螺1k ±2000dps
	
	
	if (is_low_noise_mode)
		rc |= inv_icm426xx_enable_accel_low_noise_mode(&icm_driver);//开启加表低噪声
	else
		rc |= inv_icm426xx_enable_accel_low_power_mode(&icm_driver);
	
	rc |= inv_icm426xx_enable_gyro_low_noise_mode(&icm_driver);//开启陀螺低噪声

	/* Wait Max of ICM426XX_GYR_STARTUP_TIME_US and ICM426XX_ACC_STARTUP_TIME_US*/
	(ICM426XX_GYR_STARTUP_TIME_US > ICM426XX_ACC_STARTUP_TIME_US) ? inv_icm426xx_sleep_us(ICM426XX_GYR_STARTUP_TIME_US) : inv_icm426xx_sleep_us(ICM426XX_ACC_STARTUP_TIME_US);
		
	return rc;
}

uint8_t icm42688_register[30] = {0};
uint8_t icm42688_receive_buffer[30] = {0};
struct rt_semaphore ICM42688_Semaphore;
uint8_t cnt_add = 0;

int inv_io_hal_read_reg(struct inv_icm426xx_serif *serif, uint8_t reg, uint8_t *buffer, uint32_t len) 
{
		memset(&icm42688_register[0],0,sizeof(icm42688_register));
		memset(&icm42688_receive_buffer[0],0,sizeof(icm42688_receive_buffer));
		
		ICM42652_Read_Reg(reg, buffer, len);
			
		return 0;
}

static int inv_io_hal_write_reg(struct inv_icm426xx_serif * serif, uint8_t reg, const uint8_t *buf, uint32_t len)
{
	memset(&icm42688_register[0],0,sizeof(icm42688_register));
	memset(&icm42688_receive_buffer[0],0,sizeof(icm42688_receive_buffer));
	
	ICM42688_Write_Reg(reg, *buf, len);
	
	return 0;
}

uint8_t ICM42688_transmit_buffer[2] = {0};
uint8_t ICM42688_receive_buffer[2]  = {0};

void ICM42652_Read_Reg(uint8_t  reg,\
										 uint8_t  *data,\
										 uint16_t len)
{
		uint8_t translen = 0;
		ICM42688_transmit_buffer[0] = reg | 0x80;
		ICM42688_transmit_buffer[1] = 0x00;
	
		translen = len+1;
	
		ICM42688_CS_enable;
		SPI_TransmitReceive_DMA(ICM42688_Transmit_DMA,\
														ICM42688_Receive_DMA,\
														(uint32_t)&Device->dt,\
														(uint32_t)&ICM42688_transmit_buffer,\
														(uint32_t)&ICM42688_receive_buffer,\
														translen
														);
		while(SPI1->sts_bit.bf == 1){}
		ICM42688_CS_disenable;
			
		*data = ICM42688_receive_buffer[1];
}

void ICM42688_Write_Reg(uint8_t  reg,\
										  uint8_t  data,\
											uint16_t len)
{
		uint8_t translen = 0;
		ICM42688_transmit_buffer[0] = reg & 0x7F;
		ICM42688_transmit_buffer[1] = data;
	
		translen = len+1;
	
		ICM42688_CS_enable;
	
		SPI_TransmitReceive_DMA(ICM42688_Transmit_DMA,\
														ICM42688_Receive_DMA,\
														(uint32_t)&Device->dt,\
														(uint32_t)&ICM42688_transmit_buffer,\
														(uint32_t)&ICM42688_receive_buffer,\
														translen
														);
		while(SPI1->sts_bit.bf == 1){}
		ICM42688_CS_disenable;
}



uint64_t inv_icm426xx_get_time_us()
{
	return Micros();
}

int inv_icm426xx_read_reg(struct inv_icm426xx * s, uint8_t reg, uint32_t len, uint8_t * buf)
{
	struct inv_icm426xx_serif *serif = (struct inv_icm426xx_serif *)s;
	
	if(len > serif->max_read)
		return INV_ERROR_SIZE;

	if(serif->read_reg(serif, reg, buf, len) != 0) /*inv_io_hal_read_reg;*/
		return INV_ERROR_TRANSPORT;

	return 0;
}

int inv_icm426xx_write_reg(struct inv_icm426xx * s, uint8_t reg, uint32_t len, const uint8_t * buf)
{
	// First field of struct inv_icm426xx is assumed to be a struct inv_icm426xx_serif object.
	// So let's cast s to struct inv_icm426xx_serif and ignore the rest of struct inv_icm426xx.
	struct inv_icm426xx_serif *serif = (struct inv_icm426xx_serif *)s;

	if(len > serif->max_write)
		return INV_ERROR_SIZE;

	if(serif->write_reg(serif, reg, buf, len) != 0)/*inv_io_hal_write_reg;*/
		return INV_ERROR_TRANSPORT;

	return 0;
}

float ICM42688_DataToFloat(uint8_t* p)
{
	return (int16_t)((p[1] << 8) + p[2]);
}

uint8_t icm42688_data_read_register[2];
uint8_t icm42688_buffer[28];
uint8_t icm42688_data_reiseive_fun_switch = 0;

void icm42688_get(uint8_t regi,uint32_t reserve_address)
{
	if(Icm42688_Init_Over)
	{
		icm42688_data_read_register[0] = regi |0x80;
		icm42688_data_read_register[1] = 0x00;
		
		gpio_bits_write(GPIOA,GPIO_PINS_4,FALSE);
		SPI_TransmitReceive_DMA(DMA1_CHANNEL6,\
														DMA1_CHANNEL5,\
														(uint32_t)&SPI1->dt,\
														(uint32_t)&icm42688_data_read_register[0],\
														(uint32_t)reserve_address,\
														15
														);
	}
}

ICM42688_Status_Type ICM42688;
int imu_data_lose = 0;
void icm42688_number_get()
{
	uint8_t primask = __get_PRIMASK();
	__disable_irq();
	
	if(1)
	{
		icm42688_get(0x1D,(uint32_t)&icm42688_buffer[0]);
	}
	__set_PRIMASK(primask);
}




static struct inv_icm426xx icm_driver;
struct inv_icm426xx_serif icm426xx_serif;
uint8_t Icm42688_Init_Over = 0;
uint8_t reset_cmd = 0x01;

void ConfigureInvDevices(void);
void ICM42652_INT1_DRDY(void);
void ICM42652_FSYNC_Config(void);

int iotex_icm42605_init(void)
{
	rt_sem_init(&ICM42688_Semaphore, "", 0, RT_IPC_FLAG_FIFO);
	int rc = 0;
	icm426xx_serif.context    = 0;  /* no need */
  icm426xx_serif.read_reg   = inv_io_hal_read_reg;
  icm426xx_serif.write_reg  = inv_io_hal_write_reg;
  icm426xx_serif.max_read   = 1024 ;  /* maximum number of bytes allowed per serial read */
  icm426xx_serif.max_write  = 1024 ;  /* maximum number of bytes allowed per serial write */
  icm426xx_serif.serif_type = SERIF_TYPE;
	rc = inv_icm426xx_init(&icm_driver, &icm426xx_serif, 0);
	
	inv_io_hal_write_reg(&icm426xx_serif,MPUREG_CHIP_CONFIG,&reset_cmd,1);//reset
	rt_thread_mdelay(10);
	inv_io_hal_read_reg(&icm426xx_serif,MPUREG_WHO_AM_I,&whoamI1,1);//读取芯片id
	
	if(whoamI1 != ICM42652_WHOAMI)
	{
		return 0;
	}
	//配置
	ConfigureInvDevices();
	
	//需要最后配置到BANK0，不然回读写出错
	ICM42652_Read_INT1_Config();
	
	Icm42688_Init_Over = 1;
	
	//ICM42652_FSYNC_Config();
	return rc;
}

void ConfigureInvDevices(void)
{

    uint8_t val;
    uint8_t id;

    /* 切换到 Bank0 */
    val = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &val, 1);

    /* 读取 WHO_AM_I，确认芯片 ID */
    id = 0;
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_WHO_AM_I, &id, 1);

    /* 软件复位 */
    val = 0x01;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_CHIP_CONFIG, &val, 1);
	
    rt_thread_mdelay(10);
	
    /* 复位后重新切换到 Bank0 */
    val = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &val, 1);

    /* 关闭 Accel 和 Gyro */
    val = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_PWR_MGMT_0, &val, 1);

    rt_thread_mdelay(1);

    /* 配置 Gyro 为 ±2000dps、1kHz */
    val = 0x06;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_GYRO_CONFIG0, &val, 1);

    /* 配置 Accel 为 ±8g、1kHz */
    val = 0x26;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_ACCEL_CONFIG0, &val, 1);
		
		
    /* 打开 Accel 和 Gyro 低噪声模式 */
    val = 0x0F;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_PWR_MGMT_0, &val, 1);
		
		/* Gyro 滤波开到最低，没有关掉的选项 */
		val = 0x12;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_GYRO_CONFIG1, &val, 1);
		
		/* ACC 滤波开到最低，没有关掉的选项 */
		val = 0x05;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_ACCEL_CONFIG1, &val, 1);
		
		/* ACC 和 Gyro 低通滤波开到最低，没有关掉的选项 */
		val = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_ACCEL_GYRO_CONFIG0, &val, 1);
		
    /* 等待传感器稳定 */
    rt_thread_mdelay(50);
		
		 /* 配置 INT1 输出 DRDY 数据就绪中断 */
    ICM42652_INT1_DRDY();
		
		/*配置INT2为CLKIN 32kHZ*/
		ICM42652_CLKIN_Config();

    /* 清一下 AT32 外部中断挂起标志，避免初始化残留触发 */
    exint_flag_clear(EXINT_LINE_6);
}

void ICM42652_INT1_DRDY(void)
{
    uint8_t reg;

    /* 切换到 Bank0 */
    reg = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &reg, 1);

    /* 配置 INT1：脉冲模式、推挽输出、高电平有效 */
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_CONFIG, &reg, 1);
    reg &= ~(0x01 << 2);    // INT1 脉冲模式
    reg |=  (0x01 << 1);    // INT1 推挽输出
    reg |=  (0x01 << 0);    // INT1 高电平有效
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INT_CONFIG, &reg, 1);

    /* 配置 DRDY 中断清除方式：默认读状态清除即可 */
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_CONFIG0, &reg, 1);
    reg &= ~(0x03 << 4);    // UI_DRDY_INT_CLEAR = 00
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INT_CONFIG0, &reg, 1);

    /* 配置 INT 脉冲参数，并清除 INT_ASYNC_RESET */
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_CONFIG1, &reg, 1);
    reg &= ~(0x01 << 4);    // INT_ASYNC_RESET = 0
    reg &= ~(0x01 << 6);    // INT 脉冲宽度 100us，ODR < 4kHz 用
    reg &= ~(0x01 << 5);    // 保留最小 100us 去断言时间
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INT_CONFIG1, &reg, 1);

    /* 把 UI 数据就绪中断路由到 INT1 */
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_SOURCE0, &reg, 1);
    reg |= (0x01 << 3);     // UI_DRDY_INT1_EN = 1
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INT_SOURCE0, &reg, 1);
}

void ICM42652_CLKIN_Config(void)
{
    uint8_t reg;

    /* 切到 Bank1 */
    reg = 0x01;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &reg, 1);

    /* Pin9 配置为 CLKIN 输入：PIN9_FUNCTION = 02 */
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INTF_CONFIG5_B1, &reg, 1);
    reg &= ~(0x03 << 1);
    reg |=  (0x02 << 1);
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INTF_CONFIG5_B1, &reg, 1);

    /* 切回 Bank0 */
    reg = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &reg, 1);

    reg = 0x95;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INTF_CONFIG1, &reg, 1);
	
		reg = 0x20;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INT_CONFIG0, &reg, 1);
	
		reg = 0x08;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INT_SOURCE0, &reg, 1);
		
    reg &= ~(0x08);
		reg |=  (0x08);
		inv_io_hal_write_reg(&icm426xx_serif, MPUREG_TMST_CONFIG, &reg, 1);
  
}

void ICM42652_FSYNC_Config(void)
{
    uint8_t reg;

    /* 切到 Bank1 */
    reg = 0x01;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &reg, 1);

    /* Pin9 配置为 FSYNC 输入：PIN9_FUNCTION = 01 */
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INTF_CONFIG5_B1, &reg, 1);
    reg &= ~(0x03 << 1);
    reg |=  (0x01 << 1);
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INTF_CONFIG5_B1, &reg, 1);

    /* 切回 Bank0 */
    reg = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &reg, 1);

    /* 配置 FSYNC 行为 */
   // inv_io_hal_read_reg(&icm426xx_serif, MPUREG_FSYNC_CONFIG, &reg, 1);

    /*
     * FSYNC_UI_SEL = 010
     * 把 FSYNC 标记打到 GYRO_XOUT LSB
     */
    reg &= ~(0x07 << 4);
    reg |=  (0x02 << 4);

    /*
     * FSYNC_UI_FLAG_CLEAR_SEL = 0
     * UI sensor register 更新时清除 FSYNC flag
     */
    reg &= ~(0x01 << 1);

    /*
     * FSYNC_POLARITY = 0
     * 上升沿有效
     */
    reg &= ~(0x01 << 0);

   // inv_io_hal_write_reg(&icm426xx_serif, MPUREG_FSYNC_CONFIG, &reg, 1);

    /* 建议清 INT_ASYNC_RESET */
    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_CONFIG1, &reg, 1);
    reg &= ~(0x01 << 4);
   // inv_io_hal_write_reg(&icm426xx_serif, MPUREG_INT_CONFIG1, &reg, 1);
}

void gpio_toggle(gpio_type *gpio_x, uint16_t pins)
{
    	if((gpio_x->odt & pins) != 0)
			{
					gpio_x->clr = pins;
			}
			else
			{
					gpio_x->scr = pins;
			}
}

void ICM42652_Read_INT1_Config(void)
{
    uint8_t reg;
	
		//BANK1
		reg = 0x01;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &reg, 1);
		
		inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INTF_CONFIG5_B1, &reg, 1);//MPUREG_INTF_CONFIG5_B1 = 0x
    //rt_kprintf("MPUREG_INTF_CONFIG5_B1    = 0x%02X\r\n", reg);
		
		inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INTF_CONFIG5_B1, &reg, 1);//MPUREG_INTF_CONFIG5_B1 = 0x
    //rt_kprintf("MPUREG_INTF_CONFIG5_B1    = 0x%02X\r\n", reg);
	
		//BANK0
    reg = 0x00;
    inv_io_hal_write_reg(&icm426xx_serif, MPUREG_REG_BANK_SEL, &reg, 1);

    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_CONFIG, &reg, 1);//MPUREG_INT_CONFIG = 0x03
    //rt_kprintf("INT_CONFIG    = 0x%02X\r\n", reg);

    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_CONFIG0, &reg, 1);//MPUREG_INT_CONFIG0 = 0x20
    //rt_kprintf("INT_CONFIG0   = 0x%02X\r\n", reg);

    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_CONFIG1, &reg, 1);//MPUREG_INT_CONFIG1 = 0x00
    //rt_kprintf("INT_CONFIG1   = 0x%02X\r\n", reg);

    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_SOURCE0, &reg, 1);//MPUREG_INT_SOURCE0 = 0x08
    //rt_kprintf("INT_SOURCE0   = 0x%02X\r\n", reg);

    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INT_STATUS, &reg, 1);//MPUREG_INT_STATUS = 0x39
    //rt_kprintf("INT_STATUS    = 0x%02X\r\n", reg);

    inv_io_hal_read_reg(&icm426xx_serif, MPUREG_PWR_MGMT_0, &reg, 1);//MPUREG_PWR_MGMT_0 = 0x0F
    //rt_kprintf("PWR_MGMT_0    = 0x%02X\r\n", reg);
		
		inv_io_hal_read_reg(&icm426xx_serif, MPUREG_INTF_CONFIG1, &reg, 1);//
    //rt_kprintf("PWR_MGMT_0    = 0x%02X\r\n", reg);
		
		inv_io_hal_read_reg(&icm426xx_serif, MPUREG_ACCEL_CONFIG0, &reg, 1);//
    //rt_kprintf("PWR_MGMT_0    = 0x%02X\r\n", reg);
		
		inv_io_hal_read_reg(&icm426xx_serif, MPUREG_GYRO_CONFIG0, &reg, 1);//
    //rt_kprintf("PWR_MGMT_0    = 0x%02X\r\n", reg);
}
#endif

