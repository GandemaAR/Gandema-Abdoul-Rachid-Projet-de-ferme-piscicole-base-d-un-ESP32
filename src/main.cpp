#include <Arduino.h>

#define ledPin 2

void setup() {
Serial.begin(115200);
Serial.println("Starting...");
pinMode(ledPin, OUTPUT);
digitalWrite(ledPin, LOW);
}

void loop() {
digitalWrite(ledPin, HIGH);
delay(500);
digitalWrite(ledPin, LOW);
delay(500);  
Serial.println("Blinking LED");
}

