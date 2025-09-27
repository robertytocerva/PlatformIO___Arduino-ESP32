
#include <Arduino.h>

// Pines del encoder KY-040
#define ENCODER_CLK 2 // Cambia por el pin que uses
#define ENCODER_DT 4  // Cambia por el pin que uses

volatile int encoderCount = 0;
int lastClkState;

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  lastClkState = digitalRead(ENCODER_CLK);
  Serial.println("Iniciando lectura de encoder KY-040...");
}

void loop() {
  int clkState = digitalRead(ENCODER_CLK);
  if (clkState != lastClkState) {
    if (clkState == HIGH) {
      int dtState = digitalRead(ENCODER_DT);
      if (dtState == LOW) {
        encoderCount++;
        Serial.print("Paso + | Total: ");
        Serial.println(encoderCount);
      } else {
        encoderCount--;
        Serial.print("Paso - | Total: ");
        Serial.println(encoderCount);
      }
    }
  }
  lastClkState = clkState;
  delay(1); // Pequeño delay para evitar rebotes
}