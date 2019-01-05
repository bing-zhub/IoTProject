#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

const char* host = "192.168.4.1";
int heartbeat = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin("ESP_D54736");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void postDataToServer(String PostData){
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect("192.168.4.1", httpPort)) {
    Serial.println("connection failed");
    return;
  } 
  client.println("POST /data HTTP/1.1");
  client.println("Host: 192.168.4.1");
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData);        
  delay(10);
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

void loop() {
  postDataToServer("humi="+String(37)+"&temp="+String(75));
  Serial.println("closing connection");
  delay(5000);             
}
