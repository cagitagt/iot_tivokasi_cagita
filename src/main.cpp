#include "Arduino.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(23, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(23, HIGH);
  delay(500); // this speeds up the simulation
  digitalWrite(23, LOW);
  delay(500);
}
