#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "BluetoothSerial.h" 

// ====== Variables Globales  ======
String ssid;
String password;
long usuarioId = 0;


BluetoothSerial SerialBT; 
bool bluetoothActivo = false; 

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

// [NUEVO] Función para manejar el envío por Bluetooth si no hay internet
void gestionarBluetooth(String jsonDatos) {
  // Si el BT no está encendido, lo prendemos una sola vez
  if (!bluetoothActivo) {
    Serial.println("[MODO FALLBACK] Iniciando Bluetooth...");
    // Este es el nombre que buscarás en tu celular
    SerialBT.begin("ESP32_Smart_Sensor"); 
    bluetoothActivo = true;
    Serial.println("Bluetooth Iniciado. Busca 'ESP32_Smart_Sensor' en tu app.");
  }

  // Verificar si hay un dispositivo conectado (Tu App)
  if (SerialBT.hasClient()) {
    Serial.println("Cliente Bluetooth conectado. Enviando datos...");
    // Enviamos el mismo JSON. En Android solo lees la línea y parseas.
    SerialBT.println(jsonDatos); 
  } else {
    Serial.println("Bluetooth activo pero esperando conexión de la App...");
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
  // Nota: La verificación de conexión la hacemos antes de llamar a esta función en el loop
  
  Serial.println("====== Enviando lectura vía WiFi ======");
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
  delay(1000); 

  // 1. Solicitar credenciales al usuario 
  solicitarDatosIniciales();

  // 2. Conectar al WiFi con los datos proporcionados
  conectarWifi();

  // Si no jala wifi se pone bt en fa
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No se pudo conectar a WiFi al inicio. Activando protocolo de emergencia BT.");
    gestionarBluetooth(""); 
  }

  randomSeed(esp_random());
}

void loop() {
  // 1. Crear el paquete de datos siempre 
  String jsonToSend = crearJsonLectura();

  // Verificamos conexión, si se cayó (por chingon se levanta), usamos Plan B (No resulto tan chingon)
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado o inestable.");
    
    // Mandar datos por Bluetooth
    gestionarBluetooth(jsonToSend);

    // Intentar reconectar WiFi
    /*Serial.println("Intentando recuperar WiFi...");
    conectarWifi(); 
    */
  } else {
    // Si hay WiFi, mandamos a la API
    // 2. Enviar a la API, ni moso que a mi APA 
    enviarDatosAPI(jsonToSend);
  }

  delay(30000);
}