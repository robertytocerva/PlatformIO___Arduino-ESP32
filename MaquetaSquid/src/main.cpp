#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>  

// Configuración Bluetooth HC-06 (ZS-040)

SoftwareSerial BTSerial(10, 11);  // RX, TX (HC-06 usa 9600 baudios por defecto)
#define BOTON  A0     // Puedes usar cualquier pin digital/analógico disponible
bool botonPresionado = false;  // Estado previo del botón

// Pines de actuadores
#define BUZZER 53  // Pin del buzzer
#define MOTOR A7    // Pin del motor


// Display 7 segmentos (pines originales modificados para evitar conflictos)
const int segmentos[] = {2, 3, 4, 5, 6, 7, 8, 9};  // Segmentos a-g + punto
const int digito1 = 12;  // Primer dígito (antes 10, cambiado por Bluetooth)
const int digito2 = 13;  // Segundo dígito (antes 11, cambiado por Bluetooth)
const byte numeros[10] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111   // 9
};

// Objetos globales
Servo myServo;  // Control del servo
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C (dirección 0x27, 16x2)

// Nombres a mostrar en LCD según número recibido (1-8)
const char* nombres[] = {
  "",
  "",         // 0 (no usado)
  "Juan",     // 2
  "Erick",    // 3
  "Omar",     // 4
  "Gustavo",  // 5
  "Nadia",    // 6
  "David",    // 7
  "Roberto",  // 8
  "Extra"     // 
};


// Melodía original (notas y duraciones)
#define NOTE_SOL 392
#define NOTE_LA 440
#define NOTE_SI 466
#define NOTE_DO 523
#define NOTE_RE 282
#define NOTE_MI 311
#define NOTE_FA 349

int melody[] = {
  NOTE_FA, NOTE_FA, NOTE_FA, NOTE_RE, NOTE_MI, NOTE_FA,
  NOTE_FA, NOTE_FA, NOTE_FA, NOTE_RE, NOTE_MI, NOTE_FA,
  NOTE_SOL, NOTE_SOL, NOTE_SOL, NOTE_LA, NOTE_SI, NOTE_SI,
  NOTE_SI, NOTE_SOL, NOTE_FA, NOTE_FA, NOTE_SOL, NOTE_SOL, NOTE_FA
};

int noteDurations[] = {
  200, 200, 200, 400, 100, 800,
  200, 200, 200, 400, 100, 800,
  400, 200, 300, 100, 300, 100,
  200, 100, 400, 200, 300, 100, 800
};

// Configuración inicial
void setup() {
  Serial.begin(9600);          // Monitor serial
  BTSerial.begin(115200);        // Inicializar HC-06
  Serial.println("Sistema iniciado. Envie '1' para activar motor/buzzer.");
pinMode(BOTON, INPUT_PULLUP);  // Usamos resistencia interna, botón entre pin y GND

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Esperando BT...");

  // Configurar pines
  pinMode(BUZZER, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  myServo.attach(11);  // Servo en pin 9 (evita conflictos)

  
  // Configurar display 7 segmentos
  for (int i = 0; i < 8; i++) pinMode(segmentos[i], OUTPUT);
  pinMode(digito1, OUTPUT);
  pinMode(digito2, OUTPUT);

  randomSeed(analogRead(0));  // Semilla para números aleatorios
}



// Función para tocar melodía (solo si está habilitado)
void playMelodyWhileMotorOn(int duration) {
  unsigned long startTime = millis();
  int i = 0;

  digitalWrite(MOTOR, HIGH);
  while (millis() - startTime < duration) {
    tone(BUZZER, melody[i], noteDurations[i]);
    delay(noteDurations[i] * 1.3);
    i = (i + 1) % 25;
  }
  digitalWrite(MOTOR, LOW);
  noTone(BUZZER);
}



// Funciones originales del display 7 segmentos (sin cambios)
void mostrarNumero(int num, int duracion) {
  int decenas = num / 10;
  int unidades = num % 10;
  unsigned long startTime = millis();

  while (millis() - startTime < duracion) {
    digitalWrite(digito1, LOW);
    digitalWrite(digito2, HIGH);
    for (int i = 0; i < 8; i++) digitalWrite(segmentos[i], bitRead(numeros[decenas], i));
    delay(15);

    digitalWrite(digito1, HIGH);
    digitalWrite(digito2, LOW);
    for (int i = 0; i < 8; i++) digitalWrite(segmentos[i], bitRead(numeros[unidades], i));
    delay(15);
  }
}

// Función original del servo y temporizador
void Grupo_Tiempo_Puertas() {
  myServo.write(0);  // Cerrar servo
  int grupos[] = {2, 4, 5};
  int grupo = grupos[random(0, 3)];
  mostrarNumero(grupo, 5000);

  for (int i = 10; i >= 0; i--) mostrarNumero(i, 1000);

  myServo.write(90);  // Abrir servo
  delay(10000);
  myServo.write(0);   // Volver a cerrar
}

void loop() {
  int estadoBoton = digitalRead(BOTON);

// Detecta si el botón fue presionado (flanco de bajada a subida)
if (estadoBoton == LOW && !botonPresionado) {
  botonPresionado = true;  // Marcar que fue presionado

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Juego");
  lcd.setCursor(0, 1);
  lcd.print("Iniciado");
  Serial.println("Motor Encendido");

  int motorTime = random(10000, 20000);
  playMelodyWhileMotorOn(motorTime);
  Grupo_Tiempo_Puertas();  // Lógica original del servo
  delay(100);  // Pausa para estabilidad
}

// Se reinicia cuando se suelta el botón
if (estadoBoton == HIGH && botonPresionado) {
  botonPresionado = false;
  
  if (BTSerial.available()) {  
    char dato = BTSerial.read();
    
  }

  if (Serial.available()) {  
    char dato = Serial.read();
   
    Serial.print("Enviado: ");
    Serial.println(dato);
    
   
    // Mostrar nombre en LCD para cualquier número (0-8)
    if (dato >= '2' && dato <= '8') {
      int num = dato - '0';
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Eliminado: ");
      lcd.print(num);
      lcd.setCursor(0, 1);
      lcd.print(nombres[num]);
    }

     if(dato == '1'){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Juego");
       lcd.setCursor(0, 1);
      lcd.print("Iniciado");
      Serial.println("Motor Encendido");
        int motorTime = random(10000, 20000);
       playMelodyWhileMotorOn(motorTime);

       Grupo_Tiempo_Puertas();  // Lógica original del servo
       delay(100);  // Pequeña pausa para estabilidad

    }
}

    
  }
  
}