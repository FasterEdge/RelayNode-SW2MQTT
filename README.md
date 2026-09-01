<div align="center">
  <img src="https://avatars.githubusercontent.com/u/245985800?s=200&v=4" alt="logo" width="100" />
  <h2>RelayNode-SW2MQTT</h2>
  <h3>四路隔离开关输入转 MQTT 节点</h3>
</div>

### 一、项目简介

- **用途**: 将四路干接点或开关输入转换为 Wi-Fi MQTT 状态消息，适用于边缘设备和工业开关量接入。
- **硬件工程**: 根目录 `sw2mqtt_main.eprj2` 为嘉立创 EDA 专业版工程，`BOM.md` 与 `BOM.txt` 提供物料信息。
- **固件位置**: `software/ESP8266/platformio_ide/` 包含 ESP-12F 的 PlatformIO Arduino 固件；`software/README.md` 提供固件操作说明。

### 二、硬件设计

- **主控与网络**: 使用 ESP8266 ESP-12F Wi-Fi 模组。
- **输入接口**: 四路输入通过 PC817 光耦隔离，分别连接 GPIO14、GPIO12、GPIO13 和 GPIO4；光耦集电极侧使用 10KΩ 上拉。
- **USB 与烧录**: 板载 CH340X USB-UART 和 USB-C 接口，用于供电、串口烧录及调试。
- **电源**: USB-C 5V 经 AMS1117-3.3 转换为 3.3V；板载红色电源指示灯。
- **控制与状态**: SW1 将 RST 接地实现复位；GPIO2 驱动低电平有效的蓝色状态灯。
- **PCB 状态**: BOM 文档记录该设计为约 83.8 × 48.0 mm 的双层板，34 个元件、126 个焊盘和 29 个网络，最终 PCB DRC 为 0 错误、0 警告。

### 三、固件功能

- **输入采集**: 输入为低电平有效，默认防抖时间为 30 ms；状态变化后发布对应的 `0` 或 `1`。
- **设备标识**: 设备 ID 使用 `sw2mqtt-<chip-id>`，主题基路径为 `<FE_MQTT_TOPIC_PREFIX>/<device-id>`。
- **输入主题**: `input/1` 至 `input/4` 发布开关状态，并由 `FE_MQTT_RETAIN_INPUTS` 控制保留消息。
- **可用性主题**: `availability` 使用保留的 `online` / `offline` 消息，其中 `offline` 由 MQTT LWT 提供。
- **心跳主题**: `state` 周期发布包含运行时间、Wi-Fi RSSI 和 IP 地址的 JSON 心跳，模板默认周期为 60 秒。
- **连接维护**: Wi-Fi 未连接时每 15 秒重试，MQTT 未连接时每 5 秒重试；MQTT 连接成功时状态灯常亮，断开时闪烁。

### 四、配置与构建

- **构建环境**: PlatformIO 默认环境为 `esp12f`，使用 `espressif8266` 平台、`esp12e` 板级配置和 Arduino 框架。
- **依赖**: MQTT 客户端使用 `knolleary/PubSubClient@^2.8`，并启用 Arduino lwIP2 低内存构建标志。
- **串口参数**: 监视器波特率为 115200，上传速率为 460800。
- **配置文件**: 以 `include/config.example.h` 为模板配置 Wi-Fi、MQTT、主题前缀、防抖和心跳参数；不得提交真实密钥或生产凭据。

```bash
cd software/ESP8266/platformio_ide
pio run
pio run -t upload
pio device monitor
```

- **首次烧录**: 如开发板不能自动控制启动模式，按住 GPIO0 为低电平、触发 RESET 后再释放 GPIO0，手动进入 ESP8266 bootloader。

### 五、目录说明

- **`sw2mqtt_main.eprj2`**: 嘉立创 EDA 专业版硬件工程。
- **`BOM.md` / `BOM.txt`**: 物料、网络连接和生产验证信息。
- **`software/README.md`**: 固件功能、主题和烧录步骤。
- **`software/ESP8266/platformio_ide/`**: PlatformIO 工程、配置模板和 ESP8266 源码。

### 六、许可证

- **许可协议**: 根目录 `LICENSE` 为 Apache License 2.0。
- **凭据提醒**: 许可证不改变部署凭据的保密要求；发布、共享或提交前应确认配置中不包含真实 Wi-Fi 或 MQTT 密钥。
