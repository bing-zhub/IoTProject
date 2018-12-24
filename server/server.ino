#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "SoftwareSerial.h"

const char* ssid = "ESP_D54736"; 
ESP8266WebServer server(80);
SoftwareSerial dSerial(D0, D1, false, 256);

void handleData() {
  String message = "";
  char data[255];
  if (server.arg("heartbeat")== ""){     //Parameter not found
  
    message = "heartbeat Argument not found";
  
  }else{     //Parameter found
  
    message = "heartbeat  = ";
    message += server.arg("heartbeat");     //Gets the value of the query parameter
    strcpy(data, message.c_str());
    dSerial.write(data);
    yield();
    Serial.println(data);
  }
  server.send(200, "text/plain", message);       //Response to the HTTP request
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

void loop() {
  server.handleClient();  
}
