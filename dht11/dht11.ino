#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

//Wifi Configuration
#define MQTT_VERSION MQTT_VERSION_3_1_1
const char* WIFI_SSID = "Sandbox";
const char* WIFI_PASSWORD = "googlelab";
const PROGMEM char* MQTT_CLIENT_ID = "dht11_1";
const PROGMEM char* MQTT_SERVER_IP = "115.159.98.171";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";

DHTesp dht;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void callback(char* p_topic, byte* p_payload, unsigned int p_length) {

  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      // Once connected, publish an announcement...
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
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
  client.publish("nbiot/humidity/1", data, true);
  client.publish("nbiot/temperature/1", data, true);
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());
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
