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



String estimarClima(float presion, float humedad, float temperatura, int valorLDR);

float leerPresion(float &altitud) {
    float presion = bmp.readPressure() / 100.0;
    altitud = bmp.readAltitude();
    Serial.print("Presion: ");
    Serial.print(presion);
    Serial.println(" hPa");
    Serial.print("Altitud: ");
    Serial.print(altitud);
    Serial.println(" metros");
    return presion;
}

float leerHumedad() {
    float humedad = dht.readHumidity();
    if (isnan(humedad)) {
        Serial.println("Error al leer del sensor DHT11");
        return -1;
    }
    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.println(" %");
    return humedad;
}

int leerLDR() {
    int valorLDR = analogRead(LDR);
    int umbral = 2500;
    if (valorLDR > umbral) {
        digitalWrite(LED_LDR, HIGH);
        Serial.println("Es de noche");
    } else {
        digitalWrite(LED_LDR, LOW);
        Serial.println("Es de dia");
    }
    return valorLDR;
}

float leerTemperatura() {
    Vo = analogRead(33); 
    R2 = R1 * (4095.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    float temperatura = (1.0 / (A + B * logR2 + C * logR2 * logR2 * logR2));
    temperatura = temperatura - 273.15;
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" *C");
    return temperatura;
}

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
    float altitud;
    float presion = leerPresion(altitud);
    float humedad = leerHumedad();
    int valorLDR = leerLDR();
    float temperatura = leerTemperatura();

    String clima = estimarClima(presion, humedad, temperatura, valorLDR);
    Serial.print("Estimacion de clima: ");
    Serial.println(clima);

    Serial.println("************************************************");
    delay(5000);
}

String estimarClima(float presion, float humedad, float temperatura, int valorLDR) {
  String clima = "";

  if (presion < 1000 && humedad > 70) {
    clima = "Probabilidad de lluvia";
  } else if (presion > 1020 && humedad < 50) {
    clima = "Clima despejado";
  } else if (temperatura < 15) {
    clima = "Clima frio";
  } else if (temperatura > 28) {
    clima = "Clima calido";
  } else {
    clima = "Clima templado";
  }

  if (valorLDR > 2500) {
    clima += " y es de noche";
  } else {
    clima += " y es de dia";
  }

  return clima;
}