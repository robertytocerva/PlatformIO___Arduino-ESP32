#include <Arduino.h>
#include <DHT.h> // Agrega la librería DHT


//Datso termistor NTC 10K
int Vo;
float R1 = 10000; // resistencia fija del divisor de tension
float logR2, R2, TEMPERATURA;
float A = 2.174674085e-03, B = 0.7925201838e-04, C = 7.241655684e-07;
// en la paggina es necesario indicar en r2 la temperaratura ambiente tomando encuanta que de se le 
//resta 10 para calibrar el termistor y que funcine de forma correcta

// http://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

//Datos dht11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Function prototypes
float LM35Temp();
float termistor();
float leerDHT11Temp();

void setup()
{
  Serial.begin(9600);
  dht.begin(); // Inicializa el DHT11
}

void loop()
{
  

  Serial.print("Temperatura LM35: ");
  Serial.print(leerDHT11Temp());
  Serial.println(" °C");

  Serial.print("Temperatura Termistor: ");
  Serial.print(termistor());  
  Serial.println(" °C");

  delay(2000); 
  

  
}




float termistor(){
    Vo = analogRead(33); 
  // 1023.0 para arduino || 4095.0 para ESP32 ADC de 12 bits
  R2 = R1 * (4095.0 / (float)Vo - 1.0); // conversion de tension a resistencia
  logR2 = log(R2); // logaritmo de R2 necesario para ecuacion
  TEMPERATURA = (1.0 / (A + B * logR2 + C * logR2 * logR2 * logR2)); // ecuacion Steinhart-Hart
  TEMPERATURA = TEMPERATURA - 273.15; // Kelvin a Celsius

  return TEMPERATURA;
}

float leerDHT11Temp() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Error leyendo DHT11");
    return NAN;
  }
  return t;
}