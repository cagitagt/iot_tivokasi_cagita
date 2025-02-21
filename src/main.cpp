#include "Arduino.h"

int Merah = 23;
int Hijau = 22;
int Kuning = 21;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(Merah, OUTPUT);
  pinMode(Hijau, OUTPUT);
  pinMode(Kuning, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(Merah, HIGH);
  digitalWrite(Hijau, LOW);
  digitalWrite(Kuning, LOW);
  Serial.println("Merah");
  delay(30000); // this speeds up the simulation
  digitalWrite(Merah, LOW);
  digitalWrite(Hijau, HIGH);
  digitalWrite(Kuning, LOW);
  Serial.println("Hijau");
  delay(20000);
  digitalWrite(Merah, LOW);
  digitalWrite(Hijau, LOW);
  digitalWrite(Kuning, HIGH);
  Serial.println("Kuning");
  delay(5000);
}
