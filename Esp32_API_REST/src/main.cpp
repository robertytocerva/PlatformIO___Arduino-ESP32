#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "BluetoothSerial.h"
#include <SPI.h>
#include <RF24.h>

// ====== Variables Globales ======
String ssid;
String password;
long usuarioId = 0;

BluetoothSerial SerialBT; 
bool bluetoothActivo = false; 

const char* serverUrl = "https://dull-colts-move.loca.lt/api/lecturas"; 

// ====== CONFIGURACIÓN RADIO ======
// Mantenemos pines 4 y 5 para evitar conflictos con LED/WiFi en ESP32
RF24 radio(4, 5); 
const byte address[6] = "NODE1";

// Variables para temporizadores
unsigned long lastRadioReset = 0; // Para el reinicio de 5s
unsigned long ultimoEnvioAPI = 0; // Para el envío de 30s
const unsigned long INTERVALO_REINICIO_RADIO = 5000; // 5 segundos

// Variables de lectura
bool lecturaFuego = false;
float lecturaTemp = 0.0;
float lecturaHum = 0.0;
float lecturaGas = 0.0;

const int PIN_LED = 2; 

// ================= FUNCIONES AUXILIARES =================

void limpiarSerial() {
  while (Serial.available() > 0) Serial.read();
}

void solicitarDatosIniciales() {
  Serial.println("\n====== 1. CONFIGURACIÓN USUARIO Y RED ======");
  
  Serial.println("Introduce el ID del usuario:");
  while (Serial.available() == 0) delay(100);
  usuarioId = Serial.readStringUntil('\n').toInt();
  limpiarSerial();

  Serial.println("Introduce SSID:");
  while (Serial.available() == 0) delay(100);
  ssid = Serial.readStringUntil('\n');
  ssid.trim();
  limpiarSerial();

  Serial.println("Introduce Password:");
  while (Serial.available() == 0) delay(100);
  password = Serial.readStringUntil('\n');
  password.trim();
  limpiarSerial();
}

void conectarWifiOBluetooth() {
  Serial.println("\n====== 2. ESTABLECIENDO CONEXIÓN (WiFi/BT) ======");
  
  // Intentamos WiFi Primero
  if (ssid != "") {
      Serial.print("Intentando conectar a WiFi: "); Serial.println(ssid);
      WiFi.begin(ssid.c_str(), password.c_str());
      int intentos = 0;
      // Damos 10 segundos para conectar
      while (WiFi.status() != WL_CONNECTED && intentos < 20) {
        delay(500); Serial.print("."); intentos++;
      }
  }

  // Verificamos resultado
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[ÉXITO] WiFi Conectado. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n[FALLO] No se pudo conectar a WiFi.");
    Serial.println("[MODO FALLBACK] Activando Bluetooth...");
    SerialBT.begin("ESP32_Smart_Sensor"); 
    bluetoothActivo = true;
    Serial.println("[ÉXITO] Bluetooth activo. Esperando cliente...");
  }
}

void iniciarRadio() {
  Serial.println("\n====== 3. INICIANDO RADIOFRECUENCIA ======");
  // Esta función se llama SOLO después de tener conexión
  if (radio.begin()) {
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(118);
    radio.startListening();
    Serial.println("[ÉXITO] Radio NRF24 escuchando en canal 118.");
  } else {
    Serial.println("[ERROR] No se detecta el módulo NRF24. Verifique cableado.");
  }
}

void procesarDatosRadio(char* mensaje) {
    String data = String(mensaje);
    // Parseo básico CSV
    int c1 = data.indexOf(',');
    int c2 = data.indexOf(',', c1+1);
    int c3 = data.indexOf(',', c2+1);
    
    if (c1 > 0 && c2 > 0 && c3 > 0) {
        lecturaFuego = (data.substring(0, c1).toInt() == 1);
        lecturaTemp = data.substring(c1 + 1, c2).toFloat();
        lecturaHum = data.substring(c2 + 1, c3).toFloat();
        lecturaGas = data.substring(c3 + 1).toFloat();
    }
}

String crearJsonLectura() {
  String json;
  json.reserve(200);
  json = "{\"usuarioId\":" + String(usuarioId) + 
         ",\"fuegoDetectado\":" + (lecturaFuego ? "true" : "false") +
         ",\"temperaturaC\":" + String(lecturaTemp, 1) + 
         ",\"humedadRelativa\":" + String(lecturaHum, 1) + 
         ",\"gasPpm\":" + String(lecturaGas, 1) + "}";
  return json;
}

void enviarDatos(String jsonDatos) {
  // Lógica para decidir por dónde enviar
  if (WiFi.status() == WL_CONNECTED) {
      Serial.println("--- Enviando vía WiFi (API) ---");
      WiFiClientSecure client;
      client.setInsecure(); 
      HTTPClient http;
      if (http.begin(client, serverUrl)) { 
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Bypass-Tunnel-Reminder", "true"); // Header vital
        int httpCode = http.POST(jsonDatos);
        if (httpCode > 0) Serial.printf("API Response: %d\n", httpCode);
        else Serial.printf("API Error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
      }
  } else if (bluetoothActivo) {
      Serial.println("--- Enviando vía Bluetooth ---");
      if (SerialBT.hasClient()) {
         SerialBT.println(jsonDatos);
      } else {
         Serial.println("(Bluetooth activo pero sin cliente conectado)");
      }
  }
}

// ================= SETUP =================

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  delay(1000);

  // 1. Pedir datos (bloqueante)
  solicitarDatosIniciales();

  // 2. Conectar protocolos de comunicación EXTERNA (WiFi/BT)
  conectarWifiOBluetooth();

  // 3. Activar Radio NRF24 (Al final, como pediste)
  iniciarRadio();
}

// ================= LOOP =================

void loop() {
  // --- LÓGICA DE REINICIO CADA 5 SEGUNDOS ---
  // Esto cumple tu requisito: "que se reinicie cada 5 segundos para que vuelva a leer"
  if (millis() - lastRadioReset >= INTERVALO_REINICIO_RADIO) {
      // Serial.println(">> Reiniciando escucha de Radio (5s timer) <<");
      radio.stopListening();
      delay(2); // Pequeña pausa técnica
      radio.startListening();
      lastRadioReset = millis();
  }

  // --- LECTURA DE DATOS (Siempre activa) ---
  // Aunque reiniciamos cada 5s, necesitamos verificar si llegó algo en el buffer
  if (radio.available()) {
    char mensaje[32] = ""; 
    radio.read(&mensaje, sizeof(mensaje));
    
    Serial.print("RADIO RX: ");
    Serial.println(mensaje);
    procesarDatosRadio(mensaje);
    
    // Blink indicador
    digitalWrite(PIN_LED, HIGH);
    delay(10); 
    digitalWrite(PIN_LED, LOW);
  }

  // --- ENVÍO DE DATOS (CADA 30s) ---
  if (millis() - ultimoEnvioAPI >= 30000) {
      ultimoEnvioAPI = millis();
      
      // Creamos el paquete con lo último que tengamos
      String jsonToSend = crearJsonLectura();
      
      // Enviamos a WiFi o BT según corresponda
      enviarDatos(jsonToSend);
  }
}