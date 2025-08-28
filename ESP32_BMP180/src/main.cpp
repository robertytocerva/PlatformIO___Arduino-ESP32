#include <Arduino.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

//SDA 21
//SCL 22
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("No se encuentre el sensor");
    while (true) {}
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Temperatura = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Presion = ");
  Serial.print(bmp.readPressure()); 
  Serial.println(" hPa");

  Serial.print("Altitud = ");
  Serial.print(bmp.readAltitude()); 
  Serial.println(" metros");

  Serial.println();
  delay(2000);

}

