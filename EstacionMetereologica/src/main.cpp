#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

//BMP180
Adafruit_BMP085 bmp;
//DHT11
#define DHTPIN 4     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
//LDR
#define LDR 34
#define LED_LDR 2

//Datso termistor NTC 10K
int Vo;
float R1 = 10000; // resistencia fija del divisor de tension
float logR2, R2, TEMPERATURA;
float A = 2.174674085e-03, B = 0.7925201838e-04, C = 7.241655684e-07;



void leerBMP();
void leerDHT();
void leerLDR();
void leerTermistor();

void setup() {
  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("Error en el sensor de presion");
    while (1) {}
  }

  pinMode(LED_LDR, OUTPUT);
  pinMode(LDR, INPUT);
  
}

void loop() {

  Serial.println("************************************************");
  leerBMP();
  leerDHT();
  leerLDR();
  leerTermistor();
  Serial.println("************************************************");
  delay(5000);

  
}

void leerBMP() {
  float presion = bmp.readPressure();
  float altitud = bmp.readAltitude();


  Serial.print("Presion: ");
  Serial.print(presion/100.0);
  Serial.println(" hPa");

  Serial.print("Altitud: ");
  Serial.print(altitud);
  Serial.println(" metros");
}

void leerDHT() {
  
  float humedad = dht.readHumidity();

  if (isnan(humedad)) {
    Serial.println("Error al leer del sensor DHT11");
    return;
  }

  
  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println(" %");

}

void leerLDR() {
  int valorLDR = analogRead(LDR);
  int umbral = 2500; // Ajusta este valor según tu entorno

  if (valorLDR > umbral) {
    digitalWrite(LED_LDR, HIGH); // Enciende el LED cuando hay poca luz
    Serial.println("Es de noche");
  } else {
    digitalWrite(LED_LDR, LOW); // Apaga el LED cuando hay suficiente luz
    Serial.println("Es de dia");
  }
}

void leerTermistor(){
    Vo = analogRead(33); 
  // 1023.0 para arduino || 4095.0 para ESP32 ADC de 12 bits
  R2 = R1 * (4095.0 / (float)Vo - 1.0); // conversion de tension a resistencia
  logR2 = log(R2); // logaritmo de R2 necesario para ecuacion
  TEMPERATURA = (1.0 / (A + B * logR2 + C * logR2 * logR2 * logR2)); // ecuacion Steinhart-Hart
  TEMPERATURA = TEMPERATURA - 273.15; // Kelvin a Celsius

  Serial.print("Temperatura: ");
  Serial.print(TEMPERATURA);
  Serial.println(" *C");
}