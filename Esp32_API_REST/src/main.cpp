#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ====== Variables Globales  ======
String ssid;
String password;
long usuarioId = 0;

// local tunel espero que ya no deje de funcionar, por favor, ayuda
const char* serverUrl = "https://cold-bananas-melt.loca.lt/api/lecturas"; 

// ==================================================
//               FUNCIONES AUXILIARES
// ==================================================


void limpiarSerial() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

// Esto deberia pedir usuario y el guaifai
void solicitarDatosIniciales() {
  Serial.println("\n====== CONFIGURACIÓN INICIAL ======");

  // 1. Pedir ID de Usuario
  Serial.println("Por favor, introduce el ID del usuario (número entero):");
  while (Serial.available() == 0) { delay(100); } // Esperar entrada
  String idString = Serial.readStringUntil('\n');
  idString.trim(); // Eliminar espacios o saltos de línea
  usuarioId = idString.toInt();
  Serial.print("-> ID Guardado: ");
  Serial.println(usuarioId);
  limpiarSerial();

  // 2. Pedir SSID (Nombre de la red)
  Serial.println("Introduce el nombre de la red WiFi (SSID):");
  while (Serial.available() == 0) { delay(100); }
  ssid = Serial.readStringUntil('\n');
  ssid.trim();
  Serial.print("-> SSID Guardado: ");
  Serial.println(ssid);
  limpiarSerial();

  // 3. Pedir Contraseña
  Serial.println("Introduce la contraseña del WiFi:");
  while (Serial.available() == 0) { delay(100); }
  password = Serial.readStringUntil('\n');
  password.trim();
  Serial.println("-> Contraseña Guardada: ****");
  limpiarSerial();
  
  Serial.println("===================================\n");
}

// esta es la buena para la conection 
void conectarWifi() {
  // Si ya estamos conectados, no hacemos nada, espero
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);
  
  // parseo a char para que jale 
  WiFi.begin(ssid.c_str(), password.c_str());

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) { // ~10s de intento o algo asi me digo germanio
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado exitosamente.");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nError: No se pudo conectar al WiFi. Verifique credenciales.");
  }
}

// Función para generar los datos y construir el Jhon Son 
String crearJsonLectura() {
  // Generar datos aleatorios (lógica original)
  bool fuegoDetectado = (random(0, 2) == 1);
  float temperaturaC = random(200, 400) / 10.0;
  float humedadRelativa = random(300, 800) / 10.0;
  float gasPpm = random(100, 1000);// esta wea es la que se va a cambiar por lo de erick

  
  String json;
  json.reserve(200);
  json += "{";
  json += "\"usuarioId\":"; json += usuarioId; json += ",";
  json += "\"fuegoDetectado\":"; json += (fuegoDetectado ? "true" : "false"); json += ",";
  json += "\"temperaturaC\":"; json += String(temperaturaC, 1); json += ",";
  json += "\"humedadRelativa\":"; json += String(humedadRelativa, 1); json += ",";
  json += "\"gasPpm\":"; json += String(gasPpm, 1);
  json += "}";

  return json;
}

// Función para enviar el JSON a la API
void enviarDatosAPI(String jsonDatos) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: WiFi desconectado, no se puede enviar datos.");
    return;
  }

  Serial.println("====== Enviando lectura ======");
  Serial.println(jsonDatos);

  WiFiClientSecure client;
  client.setInsecure(); 
  
  HTTPClient http;
  
  if (http.begin(client, serverUrl)) { 
    http.addHeader("Content-Type", "application/json");
    // Header vital no lo quietes que al pendejo que hizo esto ya le paso  y no quiere que le pase a nadie mas
    http.addHeader("Bypass-Tunnel-Reminder", "true");

    int httpCode = http.POST(jsonDatos);

    if (httpCode > 0) {
      Serial.printf("Código HTTP: %d\n", httpCode);
      String response = http.getString();
      Serial.println("Respuesta del servidor: " + response);
    } else {
      Serial.printf("Error en POST: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
  } else {
    Serial.println("No se pudo conectar con el servidor");
  }
}

// ==================================================
//                  SETUP Y LOOP
// ==================================================

void setup() {
  Serial.begin(115200);
  delay(1000); // Dar tiempo al serial para iniciar

  // 1. Solicitamos credenciales al usuario 
  solicitarDatosIniciales();

  // 2. Conectamos al WiFi con los datos proporcionados
  conectarWifi();

  randomSeed(esp_random());
}

void loop() {
  // Verificamos conexión, si se cayó (por chingon se levanta), reintentamos conectar
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, reintentando conexión...");
    conectarWifi();
    delay(2000);
    return;
  }

  // 1. Crear el paquete de datos
  String jsonToSend = crearJsonLectura();

  // 2. Enviar a la API, ni moso que a mi APA 
  enviarDatosAPI(jsonToSend);

  delay(30000);
}