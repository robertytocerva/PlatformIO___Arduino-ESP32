
// Ejemplo simple para sensor KY-038 usando solo el pin AO (analógico)
#include <Arduino.h>

#define KY038_PIN 34 // Cambia este pin si usas otro GPIO analógico

void setup() {
	Serial.begin(115200);
	Serial.println("KY-038 test (solo AO)");
}

void loop() {
	int valor = analogRead(KY038_PIN);
	Serial.print("Valor analógico KY-038: ");
	Serial.println(valor);
	delay(500);
}

