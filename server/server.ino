#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP_D54736"; 
bool toggle=0;
ESP8266WebServer server(80);

void handleData() {
  String message = "";
  
  if (server.arg("heartbeat")== ""){     //Parameter not found
  
  message = "heartbeat Argument not found";
  
  }else{     //Parameter found
  
  message = "heartbeat  = ";
  message += server.arg("heartbeat");     //Gets the value of the query parameter
  
  }
  Serial.println(message);
  server.send(200, "text/plain", message);       //Response to the HTTP request
}

void setup() {
  delay(200);
  Serial.begin(115200);
  pinMode(2, OUTPUT);
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
