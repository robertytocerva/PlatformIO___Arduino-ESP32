#include <Arduino.h>
// Incluimos la librería para la comunicación Bluetooth Serial
#include "BluetoothSerial.h"

// Creamos un objeto para el Bluetooth Serial
BluetoothSerial SerialBT;

// --- Configuración de Pines ---
const int pinENA = 25;
const int pinIN1 = 26;
const int pinIN2 = 27;

// --- Configuración de PWM (Control de Velocidad) ---
const int frecuenciaPWM = 5000;
const int canalPWM = 0;
const int resolucionPWM = 8;

// Variable para almacenar la velocidad actual
int velocidadMotor = 0;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32_Motor_Control");
  Serial.println("El dispositivo Bluetooth está listo para emparejar.");

  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);

  ledcSetup(canalPWM, frecuenciaPWM, resolucionPWM);
  ledcAttachPin(pinENA, canalPWM);
}

void loop() {
  if (SerialBT.available()) {
    char comando = SerialBT.read();
    Serial.print("Comando recibido: ");
    Serial.println(comando);

    switch (comando) {
      case 'R':
        Serial.println("Girando a la derecha");
        digitalWrite(pinIN1, HIGH);
        digitalWrite(pinINyhkkkkkkkkkkkkk2, LOW);
        break;
      case 'L':
        Serial.println("Girando a la izquierda");
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, HIGH);
        break;
      case '1': velocidadMotor = 25; break;
      case '2': velocidadMotor = 51; break;
      case '3': velocidadMotor = 77; break; 
      case '4': velocidadMotor = 102; break;
      case '5': velocidadMotor = 127; break;
      case '6': velocidadMotor = 153; break;
      case '7': velocidadMotor = 179; break;
      case '8': velocidadMotor = 204; break;
      case '9': velocidadMotor = 230; break;
      case 'q': velocidadMotor = 255; break;
    }

    ledcWrite(canalPWM, velocidadMotor);
    Serial.print("Velocidad establecida a: ");
    Serial.println(velocidadMotor);
  }

  
  delay(1); 
}