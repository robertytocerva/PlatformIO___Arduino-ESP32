#include <Arduino.h>
#include <WiFi.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Configuración de la matriz
#define TIPO_HARDWARE MD_MAX72XX::FC16_HW
#define NUM_DISPOSITIVOS 1
#define PIN_DATOS 23
#define PIN_CS   5
#define PIN_CLK  18

MD_MAX72XX matriz = MD_MAX72XX(TIPO_HARDWARE, PIN_DATOS, PIN_CLK, PIN_CS);

// Configuración WiFi
const char* ssid = "ITSU-PERSONAL";
const char* contrasena = "ITSU#2024";

WiFiServer servidor(80);
String emojiActual = "corazon";

// Bitmaps de emojis
const byte corazon[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

const byte sonrisa[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

const byte triste[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};

const byte guino[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10110101,
  B01000010,
  B00111100
};

const byte sorpresa[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10000001,
  B01000010,
  B00111100
};

const byte enojado[8] = {
  B00111100,
  B01000010,
  B10111101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};

const byte genial[8] = {
  B00111100,
  B01011010,
  B10100101,
  B10000001,
  B10111101,
  B10011001,
  B01000010,
  B00111100
};

const byte amor[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};

const byte risa[8] = {
  B00111100,
  B01000010,
  B10000001,
  B10100101,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

const byte lengua[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10000001,
  B10011001,
  B01000010,
  B00110100
};

// Mapeo de nombres de emojis a bitmaps
const struct {
  const char* nombre;
  const byte* bitmap;
} emojis[] = {
  {"corazon", corazon},
  {"sonrisa", sonrisa},
  {"triste", triste},
  {"guiño", guino},
  {"sorpresa", sorpresa},
  {"enojado", enojado},
  {"genial", genial},
  {"amor", amor},
  {"risa", risa},
  {"lengua", lengua}
};

void dibujarEmoji(const byte* emoji) {
  matriz.clear();
  for (int i = 0; i < 8; i++) {
    matriz.setRow(i, emoji[i]);
  }
}

void setup() {
  Serial.begin(9600);
  matriz.begin();
  matriz.clear();

  WiFi.begin(ssid, contrasena);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConectado. Dirección IP: " + WiFi.localIP().toString());

  servidor.begin();
  dibujarEmoji(corazon);  // Por defecto
}

void loop() {
  WiFiClient cliente = servidor.available();
  if (cliente) {
    Serial.println("Nuevo cliente conectado");
    String peticion = cliente.readStringUntil('\r');
    cliente.read(); // limpiar
    Serial.println(peticion);

    // Coincidencia de petición de emoji
    for (auto& e : emojis) {
      if (peticion.indexOf("/" + String(e.nombre)) != -1) {
        emojiActual = e.nombre;
        dibujarEmoji(e.bitmap);
      }
    }

    // Página HTML con botones de emoji Unicode
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<title>Mostrar Emoji</title>";
    html += "<style>button{font-size:40px;margin:10px;padding:10px;}</style></head><body>";
    html += "<h2>Selecciona un emoji para mostrar en la matriz</h2>";

    // Mapeo Unicode de emojis para los botones
    const char* caracteresEmoji[] = {
      "❤️", "😄", "😢", "😉", "😲", "😠", "😎", "💕", "😂", "😛"
    };

    for (int i = 0; i < 10; i++) {
      html += "<a href='/" + String(emojis[i].nombre) + "'>";
      html += "<button>" + String(caracteresEmoji[i]) + "</button></a>";
    }

    html += "</body></html>";

    cliente.println("HTTP/1.1 200 OK");
    cliente.println("Content-type:text/html");
    cliente.println();
    cliente.println(html);
    cliente.println();
    cliente.stop();
    Serial.println("Cliente desconectado");
  }
}


