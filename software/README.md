# RelayNode-SW2MQTT firmware

## Directory

- `ESP8266/platformio_ide/`: ESP-12F firmware for VS Code + PlatformIO, Arduino framework.

## Features

- Reads the four optocoupler outputs on GPIO14, GPIO12, GPIO13 and GPIO4.
- Active-low input interpretation and 30 ms debounce.
- Publishes each state to MQTT using retained `0`/`1` payloads.
- MQTT availability/LWT (`online`/`offline`) and periodic JSON heartbeat.
- Uses the onboard GPIO2 status LED: on while MQTT is connected, blinking otherwise.

## Configure, build and flash

1. Edit `ESP8266/platformio_ide/include/config.h` with Wi-Fi and MQTT values. `config.example.h` is the template.
2. Connect USB-C. The onboard CH340X provides the serial flashing connection.
3. Put ESP8266 into bootloader mode if required by the board: hold GPIO0 low, pulse RESET, then release GPIO0.
4. Run:

```bash
cd software/ESP8266/platformio_ide
pio run
pio run -t upload
pio device monitor
```

MQTT topics use `<FE_MQTT_TOPIC_PREFIX>/<chip-id>/`:

- `availability`: `online`/`offline`
- `input/1` ... `input/4`: retained `0` or `1`
- `state`: heartbeat JSON containing uptime, RSSI and IP

> The hardware currently exposes RESET but may not provide automatic DTR/RTS boot control. Manual GPIO0 boot mode can therefore be necessary for the first flash.
