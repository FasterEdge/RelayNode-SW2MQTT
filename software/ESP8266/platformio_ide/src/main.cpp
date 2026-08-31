#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

namespace {
constexpr uint8_t kInputCount = 4;
// Hardware net order: IN1..IN4 -> ESP8266 GPIO14, GPIO12, GPIO13, GPIO4.
constexpr uint8_t kInputPins[kInputCount] = {14, 12, 13, 4};
constexpr uint8_t kStatusLedPin = 2;  // D2, active-low on the board.

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
String deviceId;
String topicBase;
bool stableState[kInputCount] = {false, false, false, false};
bool sampledState[kInputCount] = {false, false, false, false};
uint32_t changedAt[kInputCount] = {0, 0, 0, 0};
uint32_t lastWifiAttempt = 0;
uint32_t lastMqttAttempt = 0;
uint32_t lastHeartbeat = 0;

bool readActive(uint8_t index) {
  // PC817 collector outputs have 10k pull-ups; closed/active input pulls low.
  return digitalRead(kInputPins[index]) == LOW;
}

String inputTopic(uint8_t index) {
  return topicBase + "/input/" + String(index + 1);
}

void publishInput(uint8_t index, bool force = false) {
  if (!mqtt.connected() && !force) return;
  const char *payload = stableState[index] ? "1" : "0";
  mqtt.publish(inputTopic(index).c_str(), payload, FE_MQTT_RETAIN_INPUTS);
}

void publishAllInputs() {
  for (uint8_t i = 0; i < kInputCount; ++i) publishInput(i);
}

void beginWifi() {
  if (strlen(FE_WIFI_SSID) == 0) return;
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(deviceId);
  WiFi.begin(FE_WIFI_SSID, FE_WIFI_PASSWORD);
  lastWifiAttempt = millis();
}

void maintainWifi() {
  if (strlen(FE_WIFI_SSID) == 0 || WiFi.status() == WL_CONNECTED) return;
  if (millis() - lastWifiAttempt >= 15000UL) beginWifi();
}

void maintainMqtt() {
  if (WiFi.status() != WL_CONNECTED || mqtt.connected()) return;
  if (millis() - lastMqttAttempt < 5000UL) return;
  lastMqttAttempt = millis();

  const String availability = topicBase + "/availability";
  bool ok;
  if (strlen(FE_MQTT_USERNAME) != 0) {
    ok = mqtt.connect(deviceId.c_str(), FE_MQTT_USERNAME, FE_MQTT_PASSWORD,
                      availability.c_str(), 1, true, "offline");
  } else {
    ok = mqtt.connect(deviceId.c_str(), availability.c_str(), 1, true, "offline");
  }
  if (!ok) return;

  mqtt.publish(availability.c_str(), "online", true);
  publishAllInputs();
}

void scanInputs() {
  const uint32_t now = millis();
  for (uint8_t i = 0; i < kInputCount; ++i) {
    const bool current = readActive(i);
    if (current != sampledState[i]) {
      sampledState[i] = current;
      changedAt[i] = now;
    }
    if (sampledState[i] != stableState[i] && now - changedAt[i] >= FE_DEBOUNCE_MS) {
      stableState[i] = sampledState[i];
      publishInput(i);
      Serial.printf("SW%u=%u\n", i + 1, stableState[i] ? 1 : 0);
    }
  }
}

void heartbeat() {
  if (!mqtt.connected() || millis() - lastHeartbeat < FE_HEARTBEAT_MS) return;
  lastHeartbeat = millis();
  const String topic = topicBase + "/state";
  const String payload = String("{\"uptime_ms\":") + millis() +
                         ",\"rssi\":" + WiFi.RSSI() +
                         ",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
  mqtt.publish(topic.c_str(), payload.c_str(), false);
}
}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("RelayNode-SW2MQTT ESP8266 firmware"));

  pinMode(kStatusLedPin, OUTPUT);
  digitalWrite(kStatusLedPin, HIGH);
  for (uint8_t i = 0; i < kInputCount; ++i) {
    pinMode(kInputPins[i], INPUT_PULLUP);
    sampledState[i] = stableState[i] = readActive(i);
    changedAt[i] = millis();
  }

  deviceId = "sw2mqtt-" + String(ESP.getChipId(), HEX);
  topicBase = String(FE_MQTT_TOPIC_PREFIX) + "/" + deviceId;
  mqtt.setServer(FE_MQTT_HOST, FE_MQTT_PORT);
  mqtt.setBufferSize(384);
  beginWifi();
}

void loop() {
  maintainWifi();
  maintainMqtt();
  if (mqtt.connected()) mqtt.loop();
  scanInputs();
  heartbeat();
  digitalWrite(kStatusLedPin, mqtt.connected() ? LOW : ((millis() / 500U) & 1U));
  delay(2);
}
