from devicetree import dtlib
from pathlib import Path


def generator_deta10(dt, dts_file, gnss_state, bro_state, node_is_enabled):
    spi1      = dt.get_node("/deta10/spi_bus/spi1")
    lsm6dsrtr = dt.get_node("/deta10/spi_bus/spi1/lsm6dsrtr")
    iim42652  = dt.get_node("/deta10/spi_bus/spi1/iim42652")
    spa06     = dt.get_node("/deta10/spi_bus/spi1/spa06-003")

    i2c2      = dt.get_node("/deta10/i2c_bus/i2c2")
    qmc6309   = dt.get_node("/deta10/i2c_bus/i2c2/qmc6309")

    usart1    = dt.get_node("/deta10/usart_bus/usart1")
    usart2    = dt.get_node("/deta10/usart_bus/usart2")
    usart3    = dt.get_node("/deta10/usart_bus/usart3")

    # SPI1
    spi1_tx_dma = spi1.props["tx-dma-channel"].to_string()
    spi1_rx_dma = spi1.props["rx-dma-channel"].to_string()

    spi1_clk_port = spi1.props["sclk-gpio-port"].to_string()
    spi1_clk_pin  = spi1.props["sclk-gpio-pin"].to_string()

    spi1_mosi_port = spi1.props["mosi-gpio-port"].to_string()
    spi1_mosi_pin  = spi1.props["mosi-gpio-pin"].to_string()

    spi1_miso_port = spi1.props["miso-gpio-port"].to_string()
    spi1_miso_pin  = spi1.props["miso-gpio-pin"].to_string()

    # LSM6DSRTR
    lsm6dsrtr_cs_port = lsm6dsrtr.props["cs-gpio-port"].to_string()
    lsm6dsrtr_cs_pin  = lsm6dsrtr.props["cs-gpio-pin"].to_string()

    # IIM42652
    iim42652_cs_port = iim42652.props["cs-gpio-port"].to_string()
    iim42652_cs_pin  = iim42652.props["cs-gpio-pin"].to_string()

    # SPA06-003
    spa06_cs_port = spa06.props["cs-gpio-port"].to_string()
    spa06_cs_pin  = spa06.props["cs-gpio-pin"].to_string()

    # I2C2
    i2c2_tx_dma = i2c2.props["tx-dma-channel"].to_string()
    i2c2_rx_dma = i2c2.props["rx-dma-channel"].to_string()

    i2c2_scl_port = i2c2.props["scl-gpio-port"].to_string()
    i2c2_scl_pin  = i2c2.props["scl-gpio-pin"].to_string()

    i2c2_sda_port = i2c2.props["sda-gpio-port"].to_string()
    i2c2_sda_pin  = i2c2.props["sda-gpio-pin"].to_string()

    # QMC6309
    qmc6309_sensor_addr = qmc6309.props["sensor_addr"].to_string()

    # USART1
    usart1_tx_port = usart1.props["tx-gpio-port"].to_string()
    usart1_tx_pin  = usart1.props["tx-gpio-pin"].to_string()
    usart1_rx_port = usart1.props["rx-gpio-port"].to_string()
    usart1_rx_pin  = usart1.props["rx-gpio-pin"].to_string()

    usart1_tx_dma_channel = usart1.props["tx-dma-channel"].to_string()
    usart1_rx_dma_channel = usart1.props["rx-dma-channel"].to_string()

    # USART2
    usart2_tx_port = usart2.props["tx-gpio-port"].to_string()
    usart2_tx_pin  = usart2.props["tx-gpio-pin"].to_string()
    usart2_rx_port = usart2.props["rx-gpio-port"].to_string()
    usart2_rx_pin  = usart2.props["rx-gpio-pin"].to_string()

    usart2_tx_dma_channel = usart2.props["tx-dma-channel"].to_string()
    usart2_rx_dma_channel = usart2.props["rx-dma-channel"].to_string()

    # USART3
    usart3_tx_port = usart3.props["tx-gpio-port"].to_string()
    usart3_tx_pin  = usart3.props["tx-gpio-pin"].to_string()
    usart3_rx_port = usart3.props["rx-gpio-port"].to_string()
    usart3_rx_pin  = usart3.props["rx-gpio-pin"].to_string()

    usart3_tx_dma_channel = usart3.props["tx-dma-channel"].to_string()
    usart3_rx_dma_channel = usart3.props["rx-dma-channel"].to_string()

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

#define DT_LSM6DSRTR_ENABLED             {node_is_enabled(lsm6dsrtr)}
#define DT_LSM6DSRTR_CS_GPIO_PORT        {lsm6dsrtr_cs_port}
#define DT_LSM6DSRTR_CS_GPIO_PIN         {lsm6dsrtr_cs_pin}

#define DT_IIM42652_ENABLED              {node_is_enabled(iim42652)}
#define DT_IIM42652_CS_GPIO_PORT         {iim42652_cs_port}
#define DT_IIM42652_CS_GPIO_PIN          {iim42652_cs_pin}

#define DT_SPA06_ENABLED                 {node_is_enabled(spa06)}
#define DT_SPA06_CS_GPIO_PORT            {spa06_cs_port}
#define DT_SPA06_CS_GPIO_PIN             {spa06_cs_pin}

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

#define DT_GNSS_ENABLED                  {gnss_state}
#define DT_BRO_ENABLED                   {bro_state}

#define DT_HardWare_Version              0x0001010000
#define DT_IMU_COUNT                     1

#endif /* DEVICETREE_GENERATED_H */
"""

    return header_content