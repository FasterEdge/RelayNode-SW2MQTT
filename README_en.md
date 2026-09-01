<div align="center">
  <img src="https://avatars.githubusercontent.com/u/245985800?s=200&v=4" alt="logo" width="100" />
  <h2>RelayNode-SW2MQTT</h2>
  <h3>Four-Channel Isolated Switch-to-MQTT Node</h3>
</div>

### 1. Introduction

- **Purpose**: Converts four dry-contact or switch inputs into Wi-Fi MQTT state messages for edge-device and industrial digital-input integration.
- **Hardware Project**: The root `sw2mqtt_main.eprj2` is a JLCEDA Pro project, with component information in `BOM.md` and `BOM.txt`.
- **Firmware Location**: `software/ESP8266/platformio_ide/` contains the PlatformIO Arduino firmware for ESP-12F, while `software/README.md` provides firmware operating instructions.

### 2. Hardware Design

- **Controller and Network**: Uses an ESP8266 ESP-12F Wi-Fi module.
- **Input Interface**: Four inputs are isolated by PC817 optocouplers and connected to GPIO14, GPIO12, GPIO13 and GPIO4; the optocoupler collector side uses 10 kΩ pull-ups.
- **USB and Flashing**: An onboard CH340X USB-UART and USB-C connector provide power, serial flashing and debugging.
- **Power**: USB-C 5 V is converted to 3.3 V by an AMS1117-3.3, with an onboard red power LED.
- **Control and Status**: SW1 resets the module by connecting RST to ground; GPIO2 drives the active-low blue status LED.
- **PCB Status**: The BOM documents an approximately 83.8 × 48.0 mm two-layer board with 34 components, 126 pads and 29 nets; final PCB DRC reports zero errors and zero warnings.

### 3. Firmware Features

- **Input Sampling**: Inputs are active-low with a default 30 ms debounce; each stable change publishes `0` or `1`.
- **Device Identity**: The device ID is `sw2mqtt-<chip-id>`, and the topic base is `<FE_MQTT_TOPIC_PREFIX>/<device-id>`.
- **Input Topics**: `input/1` through `input/4` publish switch states, with retention controlled by `FE_MQTT_RETAIN_INPUTS`.
- **Availability Topic**: `availability` uses retained `online` / `offline` messages, with `offline` configured as the MQTT LWT.
- **Heartbeat Topic**: `state` periodically publishes JSON containing uptime, Wi-Fi RSSI and IP address; the template default interval is 60 seconds.
- **Connection Maintenance**: Wi-Fi reconnect attempts occur every 15 seconds and MQTT reconnect attempts every 5 seconds; the status LED stays on while MQTT is connected and blinks otherwise.

### 4. Configuration and Build

- **Build Environment**: The default PlatformIO environment is `esp12f`, using the `espressif8266` platform, `esp12e` board definition and Arduino framework.
- **Dependency**: The MQTT client is `knolleary/PubSubClient@^2.8`, with the Arduino lwIP2 low-memory build flag enabled.
- **Serial Settings**: The monitor runs at 115200 baud and upload speed is 460800.
- **Configuration File**: Use `include/config.example.h` as the template for Wi-Fi, MQTT, topic prefix, debounce and heartbeat settings; never commit real secrets or production credentials.

```bash
cd software/ESP8266/platformio_ide
pio run
pio run -t upload
pio device monitor
```

- **First Flash**: If the board cannot control boot mode automatically, hold GPIO0 low, pulse RESET and then release GPIO0 to enter the ESP8266 bootloader manually.

### 5. Directory Guide

- **`sw2mqtt_main.eprj2`**: JLCEDA Pro hardware project.
- **`BOM.md` / `BOM.txt`**: Components, net connections and production verification information.
- **`software/README.md`**: Firmware features, topics and flashing procedure.
- **`software/ESP8266/platformio_ide/`**: PlatformIO project, configuration template and ESP8266 source code.

### 6. License

- **License**: The root `LICENSE` file contains the Apache License 2.0.
- **Credential Reminder**: The license does not change the confidentiality requirements for deployment credentials; verify that configuration contains no real Wi-Fi or MQTT secrets before publishing, sharing or committing it.
