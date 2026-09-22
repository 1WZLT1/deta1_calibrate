
/* 当前 devicetree_generated.h 由 "Deta100.dts" 生成 */
#ifndef DEVICETREE_GENERATED_H
#define DEVICETREE_GENERATED_H

#define DT_SPI1_ENABLED                  1
#define DT_SPI1_TX_DMA_CHANNEL           DMA1_CHANNEL6
#define DT_SPI1_RX_DMA_CHANNEL           DMA1_CHANNEL5
#define DT_SPI1_CLK_PORT                 GPIOB
#define DT_SPI1_CLK_PIN                  GPIO_PINS_3
#define DT_SPI1_MOSI_PORT                GPIOB
#define DT_SPI1_MOSI_PIN                 GPIO_PINS_5
#define DT_SPI1_MISO_PORT                GPIOB
#define DT_SPI1_MISO_PIN                 GPIO_PINS_4

#define DT_LSM6DSRTR_ENABLED             1
#define DT_LSM6DSRTR_CS_GPIO_PORT        GPIOA
#define DT_LSM6DSRTR_CS_GPIO_PIN         GPIO_PINS_15

#define DT_IIM42652_ENABLED              0
#define DT_IIM42652_CS_GPIO_PORT         GPIOA
#define DT_IIM42652_CS_GPIO_PIN          GPIO_PINS_15

#define DT_SPA06_ENABLED                 1
#define DT_SPA06_CS_GPIO_PORT            GPIOC
#define DT_SPA06_CS_GPIO_PIN             GPIO_PINS_15

#define DT_SPI2_ENABLED                  1
#define DT_SPI2_TX_DMA_CHANNEL           DMA2_CHANNEL3
#define DT_SPI2_RX_DMA_CHANNEL           DMA2_CHANNEL2
#define DT_SPI2_CLK_PORT                 GPIOA
#define DT_SPI2_CLK_PIN                  GPIO_PINS_9
#define DT_SPI2_MOSI_PORT                GPIOB
#define DT_SPI2_MOSI_PIN                 GPIO_PINS_15
#define DT_SPI2_MISO_PORT                GPIOB
#define DT_SPI2_MISO_PIN                 GPIO_PINS_14

#define DT_SCHA1633_ENABLED              1
#define DT_SCHA1633_CS_GPIO_PORT         GPIOA
#define DT_SCHA1633_CS_GPIO_PIN          GPIO_PINS_11

#define DT_I2C2_ENABLED                  1
#define DT_I2C2_TX_DMA_CHANNEL           DMA1_CHANNEL7
#define DT_I2C2_RX_DMA_CHANNEL           DMA2_CHANNEL1
#define DT_I2C2_SCL_GPIO_PORT            GPIOH
#define DT_I2C2_SCL_GPIO_PIN             GPIO_PINS_2
#define DT_I2C2_SDA_GPIO_PORT            GPIOA
#define DT_I2C2_SDA_GPIO_PIN             GPIO_PINS_12

#define DT_QMC6309_ENABLED               1
#define DT_QMC6309_SENSOR_ADDR           0x7C

#define DT_USART1_ENABLED                1
#define DT_USART1_TX_DMA_CHANNEL         DMA1_CHANNEL3
#define DT_USART1_RX_DMA_CHANNEL         DMA1_CHANNEL2
#define DT_USART1_TX_GPIO_PORT           GPIOB
#define DT_USART1_TX_GPIO_PIN            GPIO_PINS_6
#define DT_USART1_RX_GPIO_PORT           GPIOB
#define DT_USART1_RX_GPIO_PIN            GPIO_PINS_7

#define DT_USART2_ENABLED                1
#define DT_USART2_TX_DMA_CHANNEL         DMA1_CHANNEL4
#define DT_USART2_RX_DMA_CHANNEL         DMA1_CHANNEL1
#define DT_USART2_TX_GPIO_PORT           GPIOA
#define DT_USART2_TX_GPIO_PIN            GPIO_PINS_2
#define DT_USART2_RX_GPIO_PORT           GPIOA
#define DT_USART2_RX_GPIO_PIN            GPIO_PINS_3

#define DT_USART3_ENABLED                1
#define DT_USART3_TX_DMA_CHANNEL         DMA2_CHANNEL5
#define DT_USART3_RX_DMA_CHANNEL         DMA2_CHANNEL4
#define DT_USART3_TX_GPIO_PORT           GPIOB
#define DT_USART3_TX_GPIO_PIN            GPIO_PINS_10
#define DT_USART3_RX_GPIO_PORT           GPIOB
#define DT_USART3_RX_GPIO_PIN            GPIO_PINS_11

#define DT_UART4_ENABLED                 1
#define DT_UART4_TX_DMA_CHANNEL          DMA2_CHANNEL7
#define DT_UART4_RX_DMA_CHANNEL          DMA2_CHANNEL6
#define DT_UART4_TX_GPIO_PORT            GPIOA
#define DT_UART4_TX_GPIO_PIN             GPIO_PINS_0
#define DT_UART4_RX_GPIO_PORT            GPIOA
#define DT_UART4_RX_GPIO_PIN             GPIO_PINS_1

#define DT_EC600_ENABLED                 1

#define DT_TMR2_CLOCK_FREQ               288000000
#define DT_TMR2_PRESCALER_VALUE          287
#define DT_TMR2_RELOAD_VALUE             999

#define DT_GNSS_ENABLED                  1
#define DT_BRO_ENABLED                   0

/*************** UKF *********************/
#define UKF_VEL_Q                             +3.2545e-02     // +0.032544903471       0.000000350530 +0.000037342305
#define UKF_VEL_ALT_Q                         +1.4483e-01     // +0.144827254833       0.000000347510 -0.000055111229
#define UKF_POS_Q                             +7.1562e+03     // +7156.240473309331    0.000000352142 +2.727925965284749
#define UKF_POS_ALT_Q                         +5.3884e+03     // +5388.369673129109    0.000000351319 -6.187843541372100
#define UKF_ACC_BIAS_Q                        +1.3317e-03     // +0.001331748045       0.000000359470 +0.000000039113
#define UKF_GYO_BIAS_Q                        +4.5256e-09     // +0.045255679186       0.000000349060 +0.000045999290
#define UKF_QUAT_Q                            +5.4005e-04     // +0.000540045060       0.000000353882 +0.000000029711
#define UKF_PRES_ALT_Q                        +6.3105e+01     // +63.104671424320      0.000000353790 +0.0166164673283
#define UKF_ACC_BIAS_V                        +7.8673e-07     // +0.000000786725       0.000000345847 -0.000000000977
#define UKF_GYO_BIAS_V                        +7.94069e-09     // +0.000000004030       0.000000359017 +0.000000000000
#define UKF_RATE_V                            +4.7764e-08     // +0.000017538388       0.000000358096 +0.000000000397
#define UKF_VEL_V                             +2.8605e-07     // +0.000000286054       0.000000351709 +0.000000000183
#define UKF_ALT_VEL_V                         +6.8304e-08     // +0.000000068304       0.000000362348 -0.000000000050
#define UKF_GPS_POS_N                         +8.0703e-02     // +0.000008070349       0.000000353490 +0.000000005602
#define UKF_GPS_POS_M_N                       +3.0245e-01     // +0.000030245341       0.000000345021 -0.000000008396
#define UKF_GPS_ALT_N                         +1.1796e-01     // +0.000011795879       0.000000356036 -0.000000010027
#define UKF_GPS_ALT_M_N                       +3.8329e-01     // +0.000038328879       0.000000346581 +0.000000027268
#define UKF_GPS_VEL_N                         +3     // +0.176404763511       0.000000355574 -0.000094105688
#define UKF_GPS_VEL_M_N                       +1     // +0.030138272888       0.000000343584 -0.000002668997
#define UKF_GPS_VD_N                          +5     // +4.637855992835       0.000000358079 +0.000310962082
#define UKF_GPS_VD_M_N                        +2     // +0.013127146795       0.000000347978 -0.000001550944
#define UKF_ALT_N                             +9.5913e-02     // +0.095913477777       0.000000356359 -0.000049781087
#define UKF_ACC_N                             +0.0004     // +0.000063286884       0.000000342761 -0.000000022717
#define UKF_DIST_N                            +1.0e-01     // +0.009737270392       0.000000356147 +0.000009059372
#define UKF_MAG_N                             +0.00001     // +0.523549973965       0.000000500000 +0.000000000000
#define UKF_MAG_DIST_N                        +0.0001
#define UKF_DUAL_ANTS_HEADING_N_G          	  +1
#define UKF_POS_DELAY                         +2.1923e+03     // +2192.300048828125    0.000000500000 +0.000000000000125
#define UKF_VEL_DELAY                         -1.0182e+05     // -101820.000000000000  0.000000500000 +0.00000000000000000
#define UKF_ACC_ERROR_ANGLE                   +20
#define UKF_MAGYAW_GATE                       +0.008
#define GAMA                                  +2
#define UKF_GYO_BIAS_LIMIT                    +0.004
#define UKF_ZRU_N                             +1e-6f
#define UKF_ZVU_N                             +1e-4f
#define UKF_ZPU_N                             +1e-4f
#define UKF_ZRU_K_OPTION                      +0
#define UKF_ZRU_COUNT                         +50
#define UKF_GYRO_TRUN_ON_LIMIT				  +0.05

#define DT_HardWare_Version                   0x0001040000
#define DT_IMU_COUNT                          2

#endif /* DEVICETREE_GENERATED_H */
