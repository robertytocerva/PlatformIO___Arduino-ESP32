#include <Arduino.h>

#define ECHo_PIN 4
#define TRIG_PIN 2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ECHo_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  
}

void loop() {
  long tiempo, distancia;
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);  
  digitalWrite(TRIG_PIN, LOW);

  tiempo = pulseIn(ECHo_PIN, HIGH);

  distancia = tiempo/59;

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
  delay(1000);
}

