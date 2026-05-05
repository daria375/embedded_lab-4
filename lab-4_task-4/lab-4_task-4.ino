#include <Arduino.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <OneWire.h>
#include <DallasTemperature.h>

Adafruit_BMP280 bmp; 
OneWire oneWire(4);
DallasTemperature ds18(&oneWire);
const int LDR_PIN = 34;

void appendFile(const char * path, String message) {
  File file = SPIFFS.open(path, FILE_APPEND);
  if(!file) return;
  file.println(message);
  file.close();
}

void readFile(const char * path) {
  File file = SPIFFS.open(path, FILE_READ);
  if(!file) return;
  while(file.available()) Serial.write(file.read());
  file.close();
}

void setup() {
  Serial.begin(115200);
  
  if(!SPIFFS.begin(true)) { Serial.println("SPIFFS Error"); return; }
  if(!bmp.begin(0x76)) { Serial.println("BMP280 Error"); }
  ds18.begin();

  File root = SPIFFS.open("/data.csv", FILE_WRITE);
  root.println("Timestamp_ms,Temp_DS,Temp_BMP,Pressure_hPa,Light_Raw");
  root.close();
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    
    ds18.requestTemperatures();
    float t_ds = ds18.getTempCByIndex(0);
    float t_bmp = bmp.readTemperature();
    float pres = bmp.readPressure() / 100.0F;
    int light = analogRead(LDR_PIN);

    String dataRow = String(millis()) + "," + String(t_ds) + "," + 
                     String(t_bmp) + "," + String(pres) + "," + String(light);
    
    appendFile("/data.csv", dataRow);
    Serial.println("Записано: " + dataRow);
    
    static int count = 0;
    if (++count == 10) {
      Serial.println("\n--- ВМІСТ ФАЙЛУ data.csv ---");
      readFile("/data.csv");
      while(1);
    }
  }
}