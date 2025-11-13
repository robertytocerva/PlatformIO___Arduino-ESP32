

/*
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

*/
#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_TEMPLATE_ID "TMPL2f4GDN7uH"
#define BLYNK_TEMPLATE_NAME "AguaBlynk"
#define BLYNK_AUTH_TOKEN "RxxJGB_ixE0APAXt-r-dF4-FPv4NEFVn"

#define WATER_SENSOR_PIN 34  // Pin analógico donde está conectado el sensor

int valorSensor = 0;  // Variable para guardar la lectura

// Inicializa Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "ITSU-PERSONAL";  // Reemplaza con tu SSID
char pass[] = "ITSU#2024";  // Reemplaza con tu contraseña

BlynkTimer timer;

void sendSensor() {
  // Lee el valor analógico (entre 0 y 4095 en el ESP32)
  valorSensor = analogRead(WATER_SENSOR_PIN);

  // Calcula el porcentaje de nivel de agua
  int porcentaje = map(valorSensor, 0, 4095, 0, 100);

  // Muestra el porcentaje en el monitor serial
  Serial.print("Nivel de agua: ");
  Serial.print(porcentaje);
  Serial.println("%");

  // Envía el porcentaje al pin virtual V0 en Blynk
  Blynk.virtualWrite(V0, porcentaje);
}

void setup() {
  Serial.begin(115200);     // Inicia comunicación serial
  delay(1000);
  Serial.println("Lectura de sensor de agua iniciada...");

  // Conecta a WiFi y Blynk
  Blynk.begin(auth, ssid, pass);

  // Configura el temporizador para enviar datos cada 2 segundos
  timer.setInterval(2000L, sendSensor);
}

void loop() {
  Blynk.run();  // Mantiene la conexión con Blynk activa
  timer.run();  // Ejecuta las tareas programadas
}