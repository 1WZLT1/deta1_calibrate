#打包需要在终端输入以下命令
#python -m PyInstaller --clean --onefile --console --name devicetree_generator --collect-all devicetree
from devicetree import dtlib
from pathlib import Path
import sys

if len(sys.argv) < 2:
    print("错误:请传入 DTS 文件名")
    sys.exit(2)
dts_file = sys.argv[1]
dt = dtlib.DT(dts_file)

if getattr(sys, "frozen", False):
    output_dir = Path(sys.executable).resolve().parent
else:
    output_dir = Path(__file__).resolve().parent

def node_is_enabled(node):
    status = node.props.get("status")

    if status is None:
        return 1

    return 1 if status.to_string() == "enable" else 0


dts_name = Path(dts_file).stem.lower()

spi1    = dt.get_node("/deta1/spi_bus/spi1")
lsm6dsr = dt.get_node("/deta1/spi_bus/spi1/lsm6dsr")
spi2    = dt.get_node("/deta1/spi_bus/spi2")
xv7011  = dt.get_node("/deta1/spi_bus/spi2/XV7011")
usart1  = dt.get_node("/deta1/usart_bus/usart1")
spi1_tx_dma     = spi1.props["tx-dma-channel"].to_string()
spi1_rx_dma     = spi1.props["rx-dma-channel"].to_string()
spi1_clk_port   = spi1.props["sclk-gpio-port"].to_string()
spi1_clk_pin    = spi1.props["sclk-gpio-pin"].to_string()
spi1_mosi_port  = spi1.props["mosi-gpio-port"].to_string()
spi1_mosi_pin   = spi1.props["mosi-gpio-pin"].to_string()
spi1_miso_port  = spi1.props["miso-gpio-port"].to_string()
spi1_miso_pin   = spi1.props["miso-gpio-pin"].to_string()
lsm6dsr_cs_port = lsm6dsr.props["cs-gpio-port"].to_string()
lsm6dsr_cs_pin  = lsm6dsr.props["cs-gpio-pin"].to_string()
spi2_tx_dma     = spi2.props["tx-dma-channel"].to_string()
spi2_rx_dma     = spi2.props["rx-dma-channel"].to_string()
spi2_clk_port   = spi2.props["sclk-gpio-port"].to_string()
spi2_clk_pin    = spi2.props["sclk-gpio-pin"].to_string()
spi2_mosi_port  = spi2.props["mosi-gpio-port"].to_string()
spi2_mosi_pin   = spi2.props["mosi-gpio-pin"].to_string()
spi2_miso_port  = spi2.props["miso-gpio-port"].to_string()
spi2_miso_pin   = spi2.props["miso-gpio-pin"].to_string()

xv7011_cs_port = xv7011.props["cs-gpio-port"].to_string()
xv7011_cs_pin  = xv7011.props["cs-gpio-pin"].to_string()

usart1_tx_port  = usart1.props["tx-gpio-port"].to_string()
usart1_tx_pin   = usart1.props["tx-gpio-pin"].to_string()
usart1_rx_port  = usart1.props["rx-gpio-port"].to_string()
usart1_rx_pin   = usart1.props["rx-gpio-pin"].to_string()
usart1_tx_dma_channel = usart1.props["tx-dma-channel"].to_string()
usart1_rx_dma_channel = usart1.props["rx-dma-channel"].to_string()


header_content = f"""
/* 当前 devicetree_generated.h 由 "{Path(dts_file).name}" 生成 */
#ifndef DEVICETREE_GENERATED_H
#define DEVICETREE_GENERATED_H

#define DT_SPI1_ENABLED                {node_is_enabled(spi1)}
#define DT_SPI1_TX_DMA_CHANNEL         {spi1_tx_dma}
#define DT_SPI1_RX_DMA_CHANNEL         {spi1_rx_dma}
#define DT_SPI1_CLK_PORT               {spi1_clk_port}
#define DT_SPI1_CLK_PIN                {spi1_clk_pin}
#define DT_SPI1_MOSI_PORT              {spi1_mosi_port}
#define DT_SPI1_MOSI_PIN               {spi1_mosi_pin}
#define DT_SPI1_MISO_PORT              {spi1_miso_port}
#define DT_SPI1_MISO_PIN               {spi1_miso_pin}

#define DT_SPI2_ENABLED                {node_is_enabled(spi2)}
#define DT_SPI2_TX_DMA_CHANNEL         {spi2_tx_dma}
#define DT_SPI2_RX_DMA_CHANNEL         {spi2_rx_dma}
#define DT_SPI2_CLK_PORT               {spi2_clk_port}
#define DT_SPI2_CLK_PIN                {spi2_clk_pin}
#define DT_SPI2_MOSI_PORT              {spi2_mosi_port}
#define DT_SPI2_MOSI_PIN               {spi2_mosi_pin}
#define DT_SPI2_MISO_PORT              {spi2_miso_port}
#define DT_SPI2_MISO_PIN               {spi2_miso_pin}

#define DT_LSM6DSR_ENABLED             {node_is_enabled(lsm6dsr)}
#define DT_LSM6DSR_CS_GPIO_PORT        {lsm6dsr_cs_port}
#define DT_LSM6DSR_CS_GPIO_PIN         {lsm6dsr_cs_pin}

#define DT_XV7011_ENABLED              {node_is_enabled(xv7011)}
#define DT_XV7011_CS_GPIO_PORT         {xv7011_cs_port}
#define DT_XV7011_CS_GPIO_PIN          {xv7011_cs_pin}

#define DT_USART1_ENABLED              {node_is_enabled(usart1)}
#define DT_USART1_TX_DMA_CHANNEL       {usart1_tx_dma_channel}
#define DT_USART1_RX_DMA_CHANNEL       {usart1_rx_dma_channel}
#define DT_USART1_TX_GPIO_PORT         {usart1_tx_port}
#define DT_USART1_TX_GPIO_PIN          {usart1_tx_pin}
#define DT_USART1_RX_GPIO_PORT         {usart1_rx_port}
#define DT_USART1_RX_GPIO_PIN          {usart1_rx_pin}

#endif /* DEVICETREE_GENERATED_H */
"""

output_file = output_dir / "devicetree_generated.h"
output_file.write_text(header_content, encoding="utf-8")
