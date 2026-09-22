#include "sensor.h"
#include "rtio.h"

#include "LSM6DSR.h"
#include "xv7001.h"
#include "SCHA16T.h"
#include "icm42688.h"

#define timeout_tick 50

rt_mailbox_t sensor_sqe_mb;
rt_mq_t sensor_cqe_mq;

ALIGN(8)
uint8_t Sensor_Sqe_Task_Stack[512];
static struct rt_thread Sensor_Sqe_Task;

ALIGN(8)
uint8_t Sensor_Cqe_Task_Stack[512];
static struct rt_thread Sensor_Cqe_Task;

Rtio_Define_Pool(spi1,100,100);
Rtio_Define_Pool(spi2,100,100);

void RawBuffer_Input(RawBuffer_t* buffer, float value)
{
	buffer->value_count++;
	buffer->value_sum += value;
	if(value == buffer->last_value)
	{
		buffer->repeat_count++;
	}
	else
	{
		buffer->repeat_count = 0;
	}
}

uint16_t ck[512],i;
float RawBuffer_Output(RawBuffer_t* buffer)
{
	if (buffer->value_count > 0)
	{
		ck[i] = buffer->value_count;
		if(i++ == 512) i =0;
		
		float result = buffer->value_sum / buffer->value_count;
		buffer->value_sum = 0;
		buffer->value_count = 0;
		buffer->last_value = result;
		return result;
	}
	else
	{
		buffer->repeat_count++;
		return buffer->last_value;
	}
}

void Sensor_Sqe_Task_Function(void* parameter)
{
	while(1)
	{
		rtio_node_t *node;
		rt_uint32_t rtio_address;
		
		rt_mb_recv(sensor_sqe_mb, &rtio_address, RT_WAITING_FOREVER);
		
		rtio_t *_rtio;
		_rtio = (rtio_t *)rtio_address;
		
		if(_rtio->Dev_busy_State == Dev_IDLE)
		{
			node = rtio_sq_pop(_rtio);
			if(node == NULL)
			{
				continue;
			}
			_rtio->Dev_busy_State = Dev_BUSY;
			_rtio->current_node   = node;
			node->dev(_rtio, node);
			
			_rtio->start_tick = rt_tick_get();
		}
		else	if((rt_tick_get() - _rtio->start_tick) > timeout_tick) 
		{
				//asm("bkpt #0");
		}
	}
}

void Sensor_Cqe_Task_Function(void* parameter)
{
	while(1)
	{
		rtio_node_rx_t dev;
		rt_mq_recv(sensor_cqe_mq, &dev, sizeof(rtio_node_rx_t), RT_WAITING_FOREVER);
		
		if(dev.Device_Name == LSM6DSR_E)
		{
			#if (DT_LSM6DS3TR_ENABLED == 1 || DT_LSM6DSRTR_ENABLED == 1)
			LSM6DSR_Conversion(dev.rx);
			#endif
		}
		
		if(dev.Device_Name == XV7001_E)
		{
			#if(DT_XV7011_ENABLED == 1)
			XV7001_Conversion(dev.rx);
			#endif
		}
		
		if(dev.Device_Name == SCH16T_E)
		{
			#if(DT_SCHA1633_ENABLED == 1)
			SCHA16T_Conversion(dev.rx);
			#endif
		}
		
		if(dev.Device_Name == ICM42688_E)
		{
			#if (DT_IIM42652_ENABLED == 1)
			ICM42688_Conversion(dev.rx);
			#endif
		}
	}
}

uint8_t sqe_init_ok = 0;
void Sensor_Init()
{
	rtio_pool_init(spi1_sqe_pool,100,&rtio_spi1);
	rtio_pool_init(spi2_sqe_pool,100,&rtio_spi2);
	
	sensor_sqe_mb = rt_mb_create("sensor_sqe_mq",8,RT_IPC_FLAG_FIFO);
	rt_thread_init(&Sensor_Sqe_Task,"Sensor_Sqe_Task",Sensor_Sqe_Task_Function,NULL,&Sensor_Sqe_Task_Stack[0],sizeof(Sensor_Sqe_Task_Stack),5,100);
	rt_thread_startup(&Sensor_Sqe_Task);

	sensor_cqe_mq = rt_mq_create("sensor_cqe_mq",
															 sizeof(rtio_node_rx_t),
															 50,
															 RT_IPC_FLAG_FIFO);	
															 
	rt_thread_init(&Sensor_Cqe_Task,"Sensor_Cqe_Task",Sensor_Cqe_Task_Function,NULL,&Sensor_Cqe_Task_Stack[0],sizeof(Sensor_Cqe_Task_Stack),5,100);
	rt_thread_startup(&Sensor_Cqe_Task);
	sqe_init_ok = 1;
}

