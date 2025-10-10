#include <Arduino.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif


BluetoothSerial SerialBT;


const int ledPin = 2; 

void setup() {
  // Iniciar el monitor serie para depuración
  Serial.begin(115200);
  
 
  pinMode(ledPin, OUTPUT);


  SerialBT.begin("ESP32_robertytocerva"); 
  Serial.println("El dispositivo está listo para emparejar.");
}

void loop() {
 
  if (SerialBT.available()) {
    // Leer el caracter recibido
    char incomingChar = SerialBT.read();

   
    Serial.print("Recibido: ");
    Serial.println(incomingChar);


    if (incomingChar == '1') {
      digitalWrite(ledPin, HIGH); 
      Serial.println("LED Encendido");
    }
    
    if (incomingChar == '0') {
      digitalWrite(ledPin, LOW); 
      Serial.println("LED Apagado");

    }
  }
  
  delay(20);
}
