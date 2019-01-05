#include "SoftwareSerial.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char *ssid = "ESP_D54736";
ESP8266WebServer server(80);
SoftwareSerial dSerial(D0, D1, false, 256);
String humi="-1", temp="-1", dist="-1";
int heartbeat = 0;

void handleData() {
  String message = "";
  if (server.arg("humi") != "")
    humi = server.arg("humi");
  if (server.arg("temp") != "")
    temp = server.arg("temp");
  if (server.arg("dist") != "") 
    dist = server.arg("dist");
    
  publishData(humi, temp, dist);
  server.send(200, "text/plain", "get data");
  heartbeat++;
  if(heartbeat>65535)
    heartbeat = 0;
}

void publishData(String humi, String temp, String dist) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["humi"] = humi;
  root["temp"] = temp;
  root["dist"] = dist;
  root["heartbeat"] = (String)heartbeat;
  char data[200];
  root.printTo(data, root.measureLength() + 1);
  dSerial.write(data);
  yield();
  Serial.println(data);
}

void setup() {
  delay(200);
  Serial.begin(9600);
  dSerial.begin(9600);
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Server started");
}

void loop() { server.handleClient(); }
