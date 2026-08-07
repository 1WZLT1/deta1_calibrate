#include "imu_task.h"
#include "rtthread.h"

#define IMU_PRIORITY	    9

ALIGN(8)
uint8_t Imu_Task_Stack[2048];
static struct rt_thread Imu_Task;

int i = 0;
static void Imu_Task_Function(void* parameter)
{
	while(1)
	{
	}
}

void imu_task_init()
{
	rt_thread_init(&Imu_Task,"Imu_Task",Imu_Task_Function,NULL,&Imu_Task_Stack[0],sizeof(Imu_Task_Stack),IMU_PRIORITY,100);
	rt_thread_startup(&Imu_Task);
}

