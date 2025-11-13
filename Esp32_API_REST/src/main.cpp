#include <WiFi.h>
#include <HTTPClient.h>

// ====== Configuración WIFI ======
const char* ssid     = "INFINITUM14D7";
const char* password = "ChXeb3fREX";


const char* serverUrl = "http://192.168.1.87:8080/api/lecturas"; // Conexión con la API mediante ngrok

const long usuarioId = 2;
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
  // fuego_detectado: true o false
  bool fuegoDetectado = (random(0, 2) == 1);

  // temperaturaC: 20.0 a 40.0
  float temperaturaC = random(200, 400) / 10.0;

  // humedadRelativa: 30.0 a 80.0
  float humedadRelativa = random(300, 800) / 10.0;

  // gasPpm: 100 a 1000
  float gasPpm = random(100, 1000);

  // ============================
  // Construir JSON
  // ============================
  String json;
  json.reserve(200);
  json += "{";
  json += "\"usuarioId\":";
  json += usuarioId;
  json += ",";

  json += "\"fuegoDetectado\":";
  json += (fuegoDetectado ? "true" : "false");
  json += ",";

  json += "\"temperaturaC\":";
  json += String(temperaturaC, 1);
  json += ",";

  json += "\"humedadRelativa\":";
  json += String(humedadRelativa, 1);
  json += ",";

  json += "\"gasPpm\":";
  json += String(gasPpm, 1);
  json += "}";

  Serial.println("====== Enviando lectura ======");
  Serial.println(json);

  // ============================
  // Enviar POST a la API
  // ============================
  HTTPClient http;
  http.begin(serverUrl);  // http://IP_PC:8080/api/lecturas
  http.addHeader("Content-Type", "application/json");

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

  // Esperar 5 segundos antes de mandar otra lectura
  delay(10000);
}