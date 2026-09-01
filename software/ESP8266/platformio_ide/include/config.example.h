// ─────────────────────────────────────────────────────────────
// FasterEdge 开源项目
// Github: https://github.com/FasterEdge
// Gitee:  https://gitee.com/FasterEdge
// ─────────────────────────────────────────────────────────────
#pragma once

// Copy this file to include/config.h and fill in your deployment values.
#define FE_WIFI_SSID "YOUR_WIFI_SSID"
#define FE_WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define FE_MQTT_HOST "192.168.1.10"
#define FE_MQTT_PORT 1883
#define FE_MQTT_USERNAME ""
#define FE_MQTT_PASSWORD ""

// Topics become: <prefix>/<chip-id>/state, /input/1 ... /input/4.
#define FE_MQTT_TOPIC_PREFIX "fasteredge/relaynode/sw2mqtt"
#define FE_MQTT_RETAIN_INPUTS true
#define FE_DEBOUNCE_MS 30U
#define FE_HEARTBEAT_MS 60000UL
