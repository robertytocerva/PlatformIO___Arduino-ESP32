


#define BLYNK_TEMPLATE_ID "TMPL2xM8aBKoa"
#define BLYNK_TEMPLATE_NAME "Walle"
#define BLYNK_AUTH_TOKEN "d9XKx7U_MvnfQrBVGerrsv-nHEgNcHnc"

#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define DHTPIN 2      
#define DHTTYPE DHT11 

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "ITSU-PERSONAL";         
char pass[] = "ITSU#2024";     

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Error al leer del sensor DHT11");
    return;
  }

  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" °C");

  Blynk.virtualWrite(V0, h); // Humedad a V0
  Blynk.virtualWrite(V1, t); // Temperatura a V1
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, sendSensor); // Cada 2 segundos
}

void loop() {
  Blynk.run();
  timer.run();
}

