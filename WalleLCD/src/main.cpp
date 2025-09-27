
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines del HC-SR04
#define TRIG_PIN 5
#define ECHO_PIN 18

// Configuración del LCD
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Medidor de distancia");
}

float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duracion = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  float distancia = duracion * 0.034 / 2;
  return distancia;
}

void loop() {
  float distancia = medirDistancia();
  lcd.setCursor(0, 1);
  lcd.print("Distancia:     "); // Limpiar línea
  lcd.setCursor(10, 1);
  lcd.print((int)distancia);
  lcd.print(" cm");
  delay(200);
}