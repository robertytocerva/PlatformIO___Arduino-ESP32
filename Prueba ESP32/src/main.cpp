#include <Arduino.h>

#define LDR 34  
#define LED 4 

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
}

void loop() {
  int ldrValue = analogRead(LDR);

  int ledBrillo = map(ldrValue, 3000, 0, 0, 255); 
  analogWrite(LED, ledBrillo);

  delay(100);
}