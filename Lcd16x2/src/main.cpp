
#include <Arduino.h>
// Incluimos las librerías necesarias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Configuración del Display LCD ---
// Reemplaza 0x27 por la dirección I2C de tu módulo si es diferente.
// Las direcciones comunes son 0x27 o 0x3F.
const int I2C_ADDR = 0x27;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Creamos el objeto del display LCD
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

// --- Configuración del LED con PWM ---
const int ledPin = 23; // Pin con capacidad PWM (GPIO 23)

// Propiedades del PWM para el ESP32
const int pwmFrequency = 5000;  // Frecuencia en Hz
const int pwmChannel = 0;       // Canal PWM (0-15)
const int pwmResolution = 8;    // Resolución en bits (8 bits = 0-255)

// Variable para almacenar el brillo actual del LED
int ledBrightness = 128; // Inicia a un brillo intermedio (rango 0-255)

void setup() {
  // Inicializamos la comunicación serie a 115200 baudios
  Serial.begin(115200);

  // --- Inicialización del PWM ---
  // Configuramos el canal PWM con sus propiedades
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  // Asociamos el pin del LED al canal PWM configurado
  ledcAttachPin(ledPin, pwmChannel);
  // Establecemos el brillo inicial
  ledcWrite(pwmChannel, ledBrightness);

  // --- Inicialización del LCD ---
  lcd.init(); // Inicializamos el display
  lcd.backlight(); // Encendemos la luz de fondo
  lcd.setCursor(0, 0); // Posicionamos el cursor en la primera fila, primera columna
  lcd.print("Escribe algo...");
}

void loop() {
  // Verificamos si hay datos disponibles en el puerto serie
  if (Serial.available() > 0) {
    // Leemos el caracter que llega
    char incomingChar = Serial.read();

    // --- Lógica de control ---
    if (incomingChar == '0') {
      // Si el caracter es '0', limpiamos el display
      lcd.clear();
      lcd.setCursor(0, 0); // Reposicionamos el cursor
      Serial.println("Display limpiado.");

    } else if (incomingChar == '+') {
      // Si es '+', aumentamos el brillo
      ledBrightness += 15; // Aumentamos en pasos de 15
      if (ledBrightness > 255) {
        ledBrightness = 255; // No exceder el valor máximo
      }
      ledcWrite(pwmChannel, ledBrightness);
      Serial.print("Brillo aumentado a: ");
      Serial.println(ledBrightness);

    } else if (incomingChar == '-') {
      // Si es '-', disminuimos el brillo
      ledBrightness -= 15; // Disminuimos en pasos de 15
      if (ledBrightness < 0) {
        ledBrightness = 0; // No ser menor que cero
      }
      ledcWrite(pwmChannel, ledBrightness);
      Serial.print("Brillo disminuido a: ");
      Serial.println(ledBrightness);

    } else {
      // Para cualquier otro caracter, lo escribimos en el display
      lcd.write(incomingChar);
    }
  }
}