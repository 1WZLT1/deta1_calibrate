from pathlib import Path

def generator_deta1(dt, dts_file, gnss_state, bro_state,node_is_enabled):
    # 获取节点
    spi1 = dt.get_node("/deta1/spi_bus/spi1")
    lsm6ds3tr = dt.get_node("/deta1/spi_bus/spi1/lsm6ds3tr")

    i2c2 = dt.get_node("/deta1/i2c_bus/i2c2")
    qmc6309 = dt.get_node("/deta1/i2c_bus/i2c2/qmc6309")

    usart1 = dt.get_node("/deta1/usart_bus/usart1")
    usart2 = dt.get_node("/deta1/usart_bus/usart2")
    usart3 = dt.get_node("/deta1/usart_bus/usart3")

    tmr2      = dt.get_node("/deta10/tmr/tmr2")

    # SPI1
    spi1_tx_dma = spi1.props["tx-dma-channel"].to_string()
    spi1_rx_dma = spi1.props["rx-dma-channel"].to_string()

    spi1_clk_port = spi1.props["sclk-gpio-port"].to_string()
    spi1_clk_pin = spi1.props["sclk-gpio-pin"].to_string()

    spi1_mosi_port = spi1.props["mosi-gpio-port"].to_string()
    spi1_mosi_pin = spi1.props["mosi-gpio-pin"].to_string()

    spi1_miso_port = spi1.props["miso-gpio-port"].to_string()
    spi1_miso_pin = spi1.props["miso-gpio-pin"].to_string()

    # LSM6DS3TR
    lsm6ds3tr_cs_port = lsm6ds3tr.props["cs-gpio-port"].to_string()
    lsm6ds3tr_cs_pin = lsm6ds3tr.props["cs-gpio-pin"].to_string()

    # I2C2
    i2c2_tx_dma = i2c2.props["tx-dma-channel"].to_string()
    i2c2_rx_dma = i2c2.props["rx-dma-channel"].to_string()

    i2c2_scl_port = i2c2.props["scl-gpio-port"].to_string()
    i2c2_scl_pin = i2c2.props["scl-gpio-pin"].to_string()

    i2c2_sda_port = i2c2.props["sda-gpio-port"].to_string()
    i2c2_sda_pin = i2c2.props["sda-gpio-pin"].to_string()

    # QMC6309
    qmc6309_sensor_addr = qmc6309.props["sensor_addr"].to_string()

    # USART1
    usart1_tx_dma_channel = usart1.props["tx-dma-channel"].to_string()
    usart1_rx_dma_channel = usart1.props["rx-dma-channel"].to_string()

    usart1_tx_port = usart1.props["tx-gpio-port"].to_string()
    usart1_tx_pin = usart1.props["tx-gpio-pin"].to_string()

    usart1_rx_port = usart1.props["rx-gpio-port"].to_string()
    usart1_rx_pin = usart1.props["rx-gpio-pin"].to_string()

    # USART2
    usart2_tx_dma_channel = usart2.props["tx-dma-channel"].to_string()
    usart2_rx_dma_channel = usart2.props["rx-dma-channel"].to_string()

    usart2_tx_port = usart2.props["tx-gpio-port"].to_string()
    usart2_tx_pin = usart2.props["tx-gpio-pin"].to_string()

    usart2_rx_port = usart2.props["rx-gpio-port"].to_string()
    usart2_rx_pin = usart2.props["rx-gpio-pin"].to_string()

    # USART3
    usart3_tx_dma_channel = usart3.props["tx-dma-channel"].to_string()
    usart3_rx_dma_channel = usart3.props["rx-dma-channel"].to_string()

    usart3_tx_port = usart3.props["tx-gpio-port"].to_string()
    usart3_tx_pin = usart3.props["tx-gpio-pin"].to_string()

    usart3_rx_port = usart3.props["rx-gpio-port"].to_string()
    usart3_rx_pin = usart3.props["rx-gpio-pin"].to_string()

    #TMR2
    tmr2_clock_freq        = tmr2.props["clock_freq"].to_string()
    tmr2_prescaler_value   = tmr2.props["prescaler_value"].to_string()
    tmr2_auto_reload_value = tmr2.props["auto_reload_value"].to_string()

    header_content = f"""
/* 当前 devicetree_generated.h 由 "{Path(dts_file).name}" 生成 */
#ifndef DEVICETREE_GENERATED_H
#define DEVICETREE_GENERATED_H

#define DT_SPI1_ENABLED                  {node_is_enabled(spi1)}
#define DT_SPI1_TX_DMA_CHANNEL           {spi1_tx_dma}
#define DT_SPI1_RX_DMA_CHANNEL           {spi1_rx_dma}
#define DT_SPI1_CLK_PORT                 {spi1_clk_port}
#define DT_SPI1_CLK_PIN                  {spi1_clk_pin}
#define DT_SPI1_MOSI_PORT                {spi1_mosi_port}
#define DT_SPI1_MOSI_PIN                 {spi1_mosi_pin}
#define DT_SPI1_MISO_PORT                {spi1_miso_port}
#define DT_SPI1_MISO_PIN                 {spi1_miso_pin}

#define DT_LSM6DS3TR_ENABLED             {node_is_enabled(lsm6ds3tr)}
#define DT_LSM6DS3TR_CS_GPIO_PORT        {lsm6ds3tr_cs_port}
#define DT_LSM6DS3TR_CS_GPIO_PIN         {lsm6ds3tr_cs_pin}

#define DT_I2C2_ENABLED                  {node_is_enabled(i2c2)}
#define DT_I2C2_TX_DMA_CHANNEL           {i2c2_tx_dma}
#define DT_I2C2_RX_DMA_CHANNEL           {i2c2_rx_dma}
#define DT_I2C2_SCL_GPIO_PORT            {i2c2_scl_port}
#define DT_I2C2_SCL_GPIO_PIN             {i2c2_scl_pin}
#define DT_I2C2_SDA_GPIO_PORT            {i2c2_sda_port}
#define DT_I2C2_SDA_GPIO_PIN             {i2c2_sda_pin}

#define DT_QMC6309_ENABLED               {node_is_enabled(qmc6309)}
#define DT_QMC6309_SENSOR_ADDR           {qmc6309_sensor_addr}

#define DT_USART1_ENABLED                {node_is_enabled(usart1)}
#define DT_USART1_TX_DMA_CHANNEL         {usart1_tx_dma_channel}
#define DT_USART1_RX_DMA_CHANNEL         {usart1_rx_dma_channel}
#define DT_USART1_TX_GPIO_PORT           {usart1_tx_port}
#define DT_USART1_TX_GPIO_PIN            {usart1_tx_pin}
#define DT_USART1_RX_GPIO_PORT           {usart1_rx_port}
#define DT_USART1_RX_GPIO_PIN            {usart1_rx_pin}

#define DT_USART2_ENABLED                {node_is_enabled(usart2)}
#define DT_USART2_TX_DMA_CHANNEL         {usart2_tx_dma_channel}
#define DT_USART2_RX_DMA_CHANNEL         {usart2_rx_dma_channel}
#define DT_USART2_TX_GPIO_PORT           {usart2_tx_port}
#define DT_USART2_TX_GPIO_PIN            {usart2_tx_pin}
#define DT_USART2_RX_GPIO_PORT           {usart2_rx_port}
#define DT_USART2_RX_GPIO_PIN            {usart2_rx_pin}

#define DT_USART3_ENABLED                {node_is_enabled(usart3)}
#define DT_USART3_TX_DMA_CHANNEL         {usart3_tx_dma_channel}
#define DT_USART3_RX_DMA_CHANNEL         {usart3_rx_dma_channel}
#define DT_USART3_TX_GPIO_PORT           {usart3_tx_port}
#define DT_USART3_TX_GPIO_PIN            {usart3_tx_pin}
#define DT_USART3_RX_GPIO_PORT           {usart3_rx_port}
#define DT_USART3_RX_GPIO_PIN            {usart3_rx_pin}

#define DT_TMR2_CLOCK_FREQ               {tmr2_clock_freq}
#define DT_TMR2_PRESCALER_VALUE          {tmr2_prescaler_value}
#define DT_TMR2_RELOAD_VALUE             {tmr2_auto_reload_value}

#define DT_GNSS_ENABLED                  {gnss_state}

#define DT_BRO_ENABLED                   {bro_state}

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
#define UKF_GYO_BIAS_V                        +7.94069e-08     // +0.000000004030       0.000000359017 +0.000000000000
#define UKF_RATE_V                            +4.7764e-07     // +0.000017538388       0.000000358096 +0.000000000397
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
#define UKF_ZRU_N                             +1e-7f
#define UKF_ZVU_N                             +1e-4f
#define UKF_ZPU_N                             +1e-4f
#define UKF_ZRU_K_OPTION                      +0
#define UKF_ZRU_COUNT                         +50
#define UKF_GYRO_TRUN_ON_LIMIT				  +0.05

#define DT_HardWare_Version                   0x0001000000
#define DT_IMU_COUNT                          1

#endif /* DEVICETREE_GENERATED_H */
"""
    return header_content