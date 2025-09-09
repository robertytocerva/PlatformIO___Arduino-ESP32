#include <Arduino.h>
#include <ESP32Servo.h>

const int servoPin = 13;
const int potenciometroPin = 34;

// Crea el objeto Servo
Servo miServo;

void setup() {
  Serial.begin(9600);
  miServo.attach(servoPin);
}

void loop() {
  // Leer el valor del potenciómetro (rango de 0 a 4095)
  int valorPotenciometro = analogRead(potenciometroPin);
  
  // Mapear el valor del potenciómetro (0-4095) al rango del servo (0-180)
  int anguloServo = map(valorPotenciometro, 0, 4095, 0, 180);
  
  // Mover el servo a la posición calculada
  miServo.write(anguloServo);
  
  // Imprimir los valores para depuración
  Serial.print("Valor del potenciometro: ");
  Serial.print(valorPotenciometro);
  Serial.print(" -> Angulo del servo: ");
  Serial.println(anguloServo);
  
  delay(15); // Pequeña pausa
}