#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;
const int totalSamples = 100;
float rawData[totalSamples];
float filteredData[totalSamples];

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

float getMedian(float window[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (window[j] > window[j + 1]) {
                float temp = window[j];
                window[j] = window[j + 1];
                window[j + 1] = temp;
            }
        }
    }
    return window[size / 2];
}

void setup() {
    Serial.begin(115200);
    sensors.begin();
    
    Serial.println("Починаю збір 100 значень...");
    
    for (int i = 0; i < totalSamples; i++) {
        sensors.requestTemperatures();
        rawData[i] = sensors.getTempCByIndex(0);
        
        Serial.printf("Замір %d: %.2f C\n", i + 1, rawData[i]);
        delay(1000);
    }

    for (int i = 1; i < totalSamples - 1; i++) {
        float window[3] = {rawData[i-1], rawData[i], rawData[i+1]};
        filteredData[i] = getMedian(window, 3);
    }
    
    filteredData[0] = rawData[0];
    filteredData[totalSamples-1] = rawData[totalSamples-1];

    Serial.println("\n--- РЕЗУЛЬТАТИ ФІЛЬТРАЦІЇ ---");
    for (int i = 0; i < totalSamples; i++) {
        Serial.print(rawData[i]);
        Serial.print(",");
        Serial.println(filteredData[i]);
    }
}

void loop() {}