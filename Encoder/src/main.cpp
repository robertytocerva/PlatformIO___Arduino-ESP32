
#include <Arduino.h>

// Pines del encoder KY-040
#define ENCODER_CLK 2 
#define ENCODER_DT 4  

volatile int encoderCount = 0; //p ara contar los movimientos del encoder
int lastClkState; // se guarda el último estado del pin CLK

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  lastClkState = digitalRead(ENCODER_CLK);
  Serial.println("Iniciando lectura de encoder KY-040...");
}

void loop() {
  //Se le el es tado y se verifica si ha cambiado
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
  delay(1); // evitar rebotes
}