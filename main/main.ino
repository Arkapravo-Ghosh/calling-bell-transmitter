#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Environment Variables
#include "env.h"
/*
WIFI_SSID
WIFI_PASSWD
MQTT_HOST
MQTT_PORT
MQTT_USER
MQTT_PASSWD
MQTT_TOPIC
DEVICE_ID
*/

// Switch Pin
const int switchPin = D5;
int active = 0;

// WiFi and MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// Setup WiFi
void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Block 1 Ringer", MQTT_USER, MQTT_PASSWD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Publish Message to MQTT
void publishMessage(char* operation) {
  StaticJsonDocument<200> doc;
  doc["id"] = DEVICE_ID;
  doc["operation"] = operation;

  char jsonBuffer[200];
  serializeJson(doc, jsonBuffer);

  client.publish(MQTT_TOPIC, jsonBuffer);
}

void setup() {
  setup_wifi();
  client.setServer(MQTT_HOST, MQTT_PORT);
  publishMessage("off");
  pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (digitalRead(switchPin) == LOW && active == 0) {
    active = 1;
    publishMessage("on");
  } else if (digitalRead(switchPin) == HIGH && active == 1) {
    active = 0;
    publishMessage("off");
  }

  delay(100);
}
