#include <Arduino.h>
#include <SPI.h>
#include <BLEPeripheral.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN A0  // Pin where the DHT11 sensor's data pin is connected
#define DHTTYPE DHT11  // Sensor type

BLEPeripheral blePeripheral;  // Create a BLE Peripheral
BLEService envService("19b10010e8f2537e4f6cd104768a1214");  // Service UUID
BLEFloatCharacteristic temperatureCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead);
BLEFloatCharacteristic humidityCharacteristic("19b10012e8f2537e4f6cd104768a1214", BLERead | BLENotify);
//We are notifying only the humidity characteristic and then reading both values from the app
//We experienced some problems when trying to notify both characteristics at the same time from the app
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(12000);
    dht.begin();
    Serial.println("DHT Sensor initialized");

    blePeripheral.setAdvertisedServiceUuid(envService.uuid());
    blePeripheral.addAttribute(envService);
    blePeripheral.addAttribute(temperatureCharacteristic);
    blePeripheral.addAttribute(humidityCharacteristic);
    blePeripheral.setLocalName("Grup5");
    blePeripheral.begin();
}

void loop() {
    BLECentral central = blePeripheral.central();

    if (central) {
        while (central.connected()) {
            delay(10000);  // Delay for sensor to stabilize before reading

            float temperature = dht.readTemperature();
            float humidity = dht.readHumidity();

            if (!isnan(humidity) && !isnan(temperature)) {
                temperatureCharacteristic.setValue(temperature);
                humidityCharacteristic.setValue(humidity);
                Serial.print("Humidity: ");
                Serial.print(humidity);
                Serial.print("%  Temperature: ");
                Serial.print(temperature);
                Serial.println(" ºC");
            } else {
                Serial.println("Failed to read from DHT sensor!");
            }
        }
    }
}
