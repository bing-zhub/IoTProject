#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define MQTT_VERSION MQTT_VERSION_3_1_1
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__ 
#include <avr/power.h>
#endif

#define PIN            D1
#define BRIGHTNESS 50
#define NUMPIXELS      30

const PROGMEM char* MQTT_CLIENT_ID = "w1";
const PROGMEM char* MQTT_SERVER_IP = "115.159.98.171";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";
int r = 0, g = 0 , b = 0, m = -1, br = 50;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


WiFiClient wifiClient;
PubSubClient client(wifiClient);

int d = 100;

byte neopix_gamma[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

void callback(char* p_topic, byte* p_payload, unsigned int p_length) {

  String payload;

  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }

  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& mBuffer = jsonBuffer.parseObject(payload);
  if(mBuffer.containsKey("m")){
    const char* mc = mBuffer["m"];
     m = atoi(mc);
  }
  if(mBuffer.containsKey("r")){
    const char* rc = mBuffer["r"];
     r = atoi(rc);
  }
  if(mBuffer.containsKey("g")){
    const char* gc = mBuffer["g"];
    g = atoi(gc);
  }
  if(mBuffer.containsKey("b")){
    const char* bc = mBuffer["b"];
    b = atoi(bc);
  }
  if(mBuffer.containsKey("d")){
    const char* dc = mBuffer["d"];
    d = atoi(dc);
  }
  if(mBuffer.containsKey("br")){
    const char* brc = mBuffer["br"];
     br = atoi(brc);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      client.subscribe("wifi/switch/1");
      client.subscribe("wifi/color/1");
      client.subscribe("wifi/slide/1");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  Serial.begin(9600);

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

  pixels.begin();
  pixels.setBrightness(br);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  pixels.setBrightness(br);
  
  if (m == 0) {
     for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(r, g, b));
      pixels.show();
    }
  } else if (m == 1) {
    for (int i = 0; i < NUMPIXELS; i++) {

      pixels.setPixelColor(i, pixels.Color(r, g, b));

      pixels.show();

      delay(d);

    }

    for (int i = NUMPIXELS - 1; i >= 0; i--) {

      pixels.setPixelColor(i, pixels.Color(0, 0, 0));

      pixels.show();

      delay(d);
    }

  } else if (m == 2) {

    whiteOverRainbow(20, 75, 5);

  }else if(m == 3){
    pulseWhite(5);
    rainbowFade2White(3, 3, 1);
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

void pulseWhite(uint8_t wait) {
  for (int j = 0; j < 256 ; j++) {
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0, neopix_gamma[j] ) );
    }
    delay(wait);
    pixels.show();
  }

  for (int j = 255; j >= 0 ; j--) {
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0, neopix_gamma[j] ) );
    }
    delay(wait);
    pixels.show();
  }
}

void rainbowFade2White(uint8_t wait, int rainbowLoops, int whiteLoops) {
  float fadeMax = 100.0;
  int fadeVal = 0;
  uint32_t wheelVal;
  int redVal, greenVal, blueVal;

  for (int k = 0 ; k < rainbowLoops ; k ++) {

    for (int j = 0; j < 256; j++) { // 5 cycles of all colors on wheel

      for (int i = 0; i < pixels.numPixels(); i++) {

        wheelVal = Wheel(((i * 256 / pixels.numPixels()) + j) & 255);

        redVal = red(wheelVal) * float(fadeVal / fadeMax);
        greenVal = green(wheelVal) * float(fadeVal / fadeMax);
        blueVal = blue(wheelVal) * float(fadeVal / fadeMax);

        pixels.setPixelColor( i, pixels.Color( redVal, greenVal, blueVal ) );

      }


      if (k == 0 && fadeVal < fadeMax - 1) {
        fadeVal++;
      }


      else if (k == rainbowLoops - 1 && j > 255 - fadeMax ) {
        fadeVal--;
      }

      pixels.show();
      delay(wait);
    }

  }



  delay(500);


  for (int k = 0 ; k < whiteLoops ; k ++) {

    for (int j = 0; j < 256 ; j++) {

      for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0, neopix_gamma[j] ) );
      }
      pixels.show();
    }

    delay(2000);
    for (int j = 255; j >= 0 ; j--) {

      for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0, neopix_gamma[j] ) );
      }
      pixels.show();
    }
  }

  delay(500);


}

void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength ) {

  if (whiteLength >= pixels.numPixels()) whiteLength = pixels.numPixels() - 1;

  int head = whiteLength - 1;
  int tail = 0;

  int loops = 3;
  int loopNum = 0;

  static unsigned long lastTime = 0;


  while (true) {
    for (int j = 0; j < 256; j++) {
      for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        if ((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) ) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0, 255 ) );
        }
        else {
          pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
        }

      }

      if (millis() - lastTime > whiteSpeed) {
        head++;
        tail++;
        if (head == pixels.numPixels()) {
          loopNum++;
        }
        lastTime = millis();
      }

      if (loopNum == loops) return;

      head %= pixels.numPixels();
      tail %= pixels.numPixels();
      pixels.show();
      delay(wait);
    }
  }

}

void fullWhite() {

  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0, 255 ) );
  }
  pixels.show();
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3, 0);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}

uint8_t green(uint32_t c) {
  return (c >> 8);
}

uint8_t blue(uint32_t c) {
  return (c);
}
