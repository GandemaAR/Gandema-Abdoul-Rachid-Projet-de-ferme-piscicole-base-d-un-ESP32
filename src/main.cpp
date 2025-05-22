#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "ESP32-AP";
const char* password = "123456789";
IPAddress local_ip  (192,168,4,1);
IPAddress getway    (192,168,4,1);
IPAddress subnet    (255,255,255,0);

void setup() {
Serial.begin(115200);
Serial.println("Starting...");
WiFi.softAP(ssid,password);
WiFi.softAPConfig(local_ip,getway,subnet);
}

void loop() {

}

