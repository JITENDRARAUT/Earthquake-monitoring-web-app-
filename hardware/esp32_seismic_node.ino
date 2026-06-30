#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
a
const char* WIFI_SSID = "YOUR_WIFI_NETWORK";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* TARGET_TOPIC = "iot/earthquake/seismic/data";

WiFiClient netClient;
PubSubClient mqttClient(netClient);
Adafruit_MPU6050 sensorModule;

void connectNetwork() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(400); }
}

void enforceConnections() {
  if (!mqttClient.connected()) {
    while (!mqttClient.connected()) {
      if (mqttClient.connect("ESP32_Seismic_Processor")) {
        mqttClient.subscribe(TARGET_TOPIC);
      } else {
        delay(3000);
      }
    }
  }
}

void setup() {
  Wire.begin(21, 22); // Target default I2C pins SDA=21, SCL=22
  connectNetwork();
  sensorModule.begin();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectNetwork();
  enforceConnections();
  mqttClient.loop();

  sensors_event_t axisAccel, axisGyro, internalTemp;
  sensorModule.getEvent(&axisAccel, &axisGyro, &internalTemp);

  // Compute 3D Euclidean magnitude vector
  float magnitude = sqrt(pow(axisAccel.acceleration.x, 2) + pow(axisAccel.acceleration.y, 2) + pow(axisAccel.acceleration.z, 2));
  float deviation = abs(magnitude - 9.81); // Compare against baseline flat earth gravity reference (~9.81m/s^2)

  StaticJsonDocument<250> jsonPayload;
  jsonPayload["x"] = axisAccel.acceleration.x;
  jsonPayload["y"] = axisAccel.acceleration.y;
  jsonPayload["z"] = axisAccel.acceleration.z;
  jsonPayload["deviation"] = deviation;
  jsonPayload["status"] = (deviation > 3.0) ? "EARTHQUAKE_ALERT" : "STABLE";

  char transportBuffer[250];
  serializeJson(jsonPayload, transportBuffer);
  
  mqttClient.publish(TARGET_TOPIC, transportBuffer);
  delay(150); // High polling sample stream output rate (150ms intervals)
}


