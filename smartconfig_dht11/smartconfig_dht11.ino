#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "DHTesp.h"

const PROGMEM char* MQTT_CLIENT_ID = "dht11_1";
const PROGMEM char* MQTT_SERVER_IP = "115.159.98.171";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";
DHTesp dht;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishData(float temperature, float humidity, float heatIndex) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = (String)temperature;
  root["humidity"] = (String)humidity;
  root["heatIndex"] = (String)heatIndex;
  root.prettyPrintTo(Serial);
  char data[200];
  root.printTo(data, root.measureLength() + 1);
  client.publish("wifi/humidity/1", data, true);
  client.publish("wifi/temperature/1", data, true);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  
  WiFi.mode(WIFI_STA);
  delay(500);

  WiFi.beginSmartConfig();
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(WiFi.smartConfigDone()){
      Serial.println();
      Serial.println("WiFi Smart Config Done.");
      break;
    }
  }
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
  dht.setup(D0, DHTesp::DHT11);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  
  
  publishData(temperature, humidity, heatIndex);
  delay(500);
  client.loop();
}
