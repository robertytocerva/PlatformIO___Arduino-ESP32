#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h> // [1] Nueva librería necesaria para HTTPS

// ====== Configuración WIFI ======
const char* ssid     = "INFINITUM14D7";
const char* password = "ChXeb3fREX";

// [2] IMPORTANTE: Pon aquí tu URL de LocalTunnel (empieza con https://)
// Ejemplo: "https://tani-gato-22.loca.lt/api/lecturas"
const char* serverUrl = "https://rude-lizards-bathe.loca.lt/api/lecturas"; 

const long usuarioId = 3;
// ============================

void conectarWifi() {
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) { // ~20s
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado");
    Serial.print("IP de la ESP32: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar al WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  conectarWifi();

  // Semilla para números aleatorios
  randomSeed(esp_random());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, reintentando...");
    conectarWifi();
    delay(2000);
    return;
  }

  // ============================
  // Generar datos aleatorios
  // ============================
  bool fuegoDetectado = (random(0, 2) == 1);
  float temperaturaC = random(200, 400) / 10.0;
  float humedadRelativa = random(300, 800) / 10.0;
  float gasPpm = random(100, 1000);

  // ============================
  // Construir JSON
  // ============================
  String json;
  json.reserve(200);
  json += "{";
  json += "\"usuarioId\":"; json += usuarioId; json += ",";
  json += "\"fuegoDetectado\":"; json += (fuegoDetectado ? "true" : "false"); json += ",";
  json += "\"temperaturaC\":"; json += String(temperaturaC, 1); json += ",";
  json += "\"humedadRelativa\":"; json += String(humedadRelativa, 1); json += ",";
  json += "\"gasPpm\":"; json += String(gasPpm, 1);
  json += "}";

  Serial.println("====== Enviando lectura ======");
  Serial.println(json);

  // ============================
  // Enviar POST a la API (Adaptado para LocalTunnel)
  // ============================
  
  // 1. Crear cliente seguro e ignorar certificados (Vital para LocalTunnel)
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  
  // 2. Iniciar conexión pasando el cliente seguro
  if (http.begin(client, serverUrl)) { 
    
    http.addHeader("Content-Type", "application/json");
    
    // [3] HEADER CLAVE: Esto evita que LocalTunnel bloquee la petición
    http.addHeader("Bypass-Tunnel-Reminder", "true");

    int httpCode = http.POST(json);

    if (httpCode > 0) {
      Serial.printf("Código HTTP: %d\n", httpCode);
      String response = http.getString();
      Serial.println("Respuesta del servidor:");
      Serial.println(response);
    } else {
      Serial.printf("Error en POST: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
  } else {
    Serial.println("No se pudo conectar con el servidor");
  }

  // Esperar 10 segundos antes de mandar otra lectura
  delay(40000);
}