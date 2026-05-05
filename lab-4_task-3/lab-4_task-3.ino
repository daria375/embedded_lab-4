#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

struct KalmanFilter {
  float q; // Шум процесу
  float r; // Шум вимірювання
  float x; // Оцінене значення
  float p; // Похибка оцінки
  float k; // Коефіцієнт Кальмана
};

// Параметри: q=0.01 (плавність), r=0.1 (довіра до датчика)
KalmanFilter kf = {0.01, 0.1, 20.0, 1.0, 0};

float kalmanUpdate(float measurement) {
  kf.p = kf.p + kf.q;
  kf.k = kf.p / (kf.p + kf.r);
  kf.x = kf.x + kf.k * (measurement - kf.x);
  kf.p = (1 - kf.k) * kf.p;
  return kf.x;
}

int measurementCount = 0;
const int maxMeasurements = 100;

void setup() {
  Serial.begin(115200);
  sensors.begin();
  
  Serial.println("--- ПОЧАТОК ВИМІРЮВАННЯ (100 значень) ---");
  Serial.println("№ | Сире значення | Фільтр Кальмана");

  sensors.requestTemperatures();
  kf.x = sensors.getTempCByIndex(0);
}

void loop() {
  if (measurementCount < maxMeasurements) {
    sensors.requestTemperatures();
    float raw = sensors.getTempCByIndex(0);
    float filtered = kalmanUpdate(raw);

    measurementCount++;

    Serial.print("Замір ");
    Serial.print(measurementCount);
    Serial.print(": ");
    Serial.print(raw);
    Serial.print(" C | Кальман: ");
    Serial.print(filtered);
    Serial.println(" C");

    delay(1000);
  } 
  else if (measurementCount == maxMeasurements) {
    Serial.println("--- ВИМІРЮВАННЯ ЗАВЕРШЕНО ---");
    measurementCount++;
  }
}