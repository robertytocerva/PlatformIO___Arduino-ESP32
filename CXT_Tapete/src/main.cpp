#include <Arduino.h>
#include <Keyboard.h> 

// Pines FSR
#define FSRq A5
#define FSRa A4
#define FSRw A3
#define FSRs A2
#define FSRd A1
#define FSRe A0

// Pines RGB
const int pinRojo = 9;
const int pinVerde = 10;
const int pinAzul = 11;

void setup() {
  pinMode(FSRa, INPUT);
  pinMode(FSRw, INPUT);
  pinMode(FSRs, INPUT);
  pinMode(FSRd, INPUT);
  pinMode(FSRq, INPUT);
  pinMode(FSRe, INPUT);

  pinMode(pinRojo, OUTPUT);
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAzul, OUTPUT);

  Serial.begin(9600);
}

// Encender color específico
void encenderColor(int rojo, int verde, int azul) {
  analogWrite(pinRojo, rojo);
  analogWrite(pinVerde, verde);
  analogWrite(pinAzul, azul);
}

// Parpadeo en morado y variantes
void parpadearMorado() {
  encenderColor(255, 0, 255); // Morado fuerte
  delay(100);
  encenderColor(200, 0, 150); // Tono intermedio
  delay(100);
  encenderColor(100, 0, 100); // Tono más suave
  delay(100);
  encenderColor(0, 255, 0);
  delay(100); // Verde
  encenderColor(0, 0, 0);     // Apagar
}

void loop() {
  int fsrValueA = analogRead(FSRa);
  int fsrValueW = analogRead(FSRw);
  int fsrValueS = analogRead(FSRs);
  int fsrValueD = analogRead(FSRd);
  int fsrValueQ = analogRead(FSRq);
  int fsrValueE = analogRead(FSRe);  

  if (fsrValueA > 1000) {
    Keyboard.press('a');
    parpadearMorado();
    Keyboard.release('a');
  }
  if (fsrValueW > 1000) {
    Keyboard.press('w');
    parpadearMorado();
    Keyboard.release('w');
  }
  if (fsrValueS > 1000) {
    Keyboard.press('s');
    parpadearMorado();
    Keyboard.release('s');
  }
  if (fsrValueD > 1000) {
    Keyboard.press('d');
    parpadearMorado();
    Keyboard.release('d');
  }
  if (fsrValueQ > 1000) {
    Keyboard.press('q');
    parpadearMorado();
    Keyboard.release('q');
  }
  if (fsrValueE > 1000) {
    Keyboard.press('e');
    parpadearMorado();
    Keyboard.release('e');
  }
}
