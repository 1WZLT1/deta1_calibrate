#ifndef __xv7001_h
#define __xv7001_h

#include "devicetree_generated.h"
#if (DT_XV7011_ENABLED == 1)

#include "stdint.h"
#include "rtio.h"
#include "sensor.h"


#define xv7001_CS_enable     gpio_bits_write(GPIOA,GPIO_PINS_11,FALSE);    
#define xv7001_CS_disable    gpio_bits_write(GPIOA,GPIO_PINS_11,TRUE);

typedef struct
{
    RawBuffer_t BufGyro;
    RawBuffer_t BufTemp;
		float GyroScale_xv7011;
    float Gyro_xv7011;
    float Temp;
    uint8_t GyroValid;
    uint8_t TempValid;
    volatile uint64_t lastUpdate;
    volatile int Initialized;
    struct rt_semaphore Semaphore;
} XV7011_Status_Type;

void xv7001_Init(void);
void xv7001_callback(rtio_t *rtio);
void XV7001_Conversion(uint8_t *Data_Receive);

extern uint8_t XV7001_Data_Recive[4];
extern float xv7001_x_gyro_raw;
extern XV7011_Status_Type XV7011;
#endif
#endif

