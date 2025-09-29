

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dirección I2C y dimensiones del LCD 16x2
const int I2C_ADDR = 0x27;      // Dirección I2C del módulo LCD
const int LCD_COLS = 16;        // Número de columnas del LCD
const int LCD_ROWS = 2;         // Número de filas del LCD

// Instancia del objeto LCD usando I2C
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

// Configuración del pin y parámetros PWM para el control de brillo del LED
const int ledPin = 23;          
const int pwmFrequency = 5000;  // Frecuencia PWM en Hz
const int pwmChannel = 0;       
const int pwmResolution = 8;    // Resolución PWM en bits (8 bits = 0-255)
int ledBrightness = 128;        


void setup() {
  Serial.begin(115200); // Comunicación serial para comandos

  // Inicialización PWM para el LED
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(ledPin, pwmChannel);
  ledcWrite(pwmChannel, ledBrightness);

  // Inicialización del LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Escribe algo...");
}


void loop() {
  if (Serial.available() > 0) {
    char incomingChar = Serial.read(); // Lee el carácter recibido

    // Comando para limpiar el display
    if (incomingChar == '0') {
      lcd.clear();
      lcd.setCursor(0, 0);
      Serial.println("Display limpiado.");
    }
    // Comando para aumentar el brillo del LED
    else if (incomingChar == '+') {
      ledBrightness += 15;
      if (ledBrightness > 255) ledBrightness = 255;
      ledcWrite(pwmChannel, ledBrightness);
      Serial.print("Brillo aumentado a: ");
      Serial.println(ledBrightness);
    }
    // Comando para disminuir el brillo del LED
    else if (incomingChar == '-') {
      ledBrightness -= 15;
      if (ledBrightness < 0) ledBrightness = 0;
      ledcWrite(pwmChannel, ledBrightness);
      Serial.print("Brillo disminuido a: ");
      Serial.println(ledBrightness);
    }
    // Cualquier otro carácter se muestra en el LCD
    else {
      lcd.write(incomingChar);
    }
  }
}