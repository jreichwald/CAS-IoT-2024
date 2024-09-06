// Example testing sketch for various DHT humidity/temperature sensors written by ladyada
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN 4     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
}


void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  Serial.print(h);
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  Serial.print(t);
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  Serial.print(f);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

   // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  // JSON-Datenstruktur erstellen
  StaticJsonDocument<200> jsonDoc;
  jsonDoc[0] = t;
  jsonDoc[1] = h;

  // JSON-Daten in einen String konvertieren
  String jsonData;
  serializeJson(jsonDoc, jsonData);

  // Ausgabe der JSON-Daten über die serielle Schnittstelle
  Serial.print("Sende JSON-Daten: ");
  Serial.println(jsonData);

  // JSON-String in Byte-Array konvertieren
  int jsonLength = jsonData.length();
  uint8_t payload[jsonLength];
  jsonData.getBytes(payload, jsonLength + 1);
  // Ausgabe des Byte-Arrays für Debug-Zwecke
  Serial.print("Byte-Array: ");
  for (int i = 0; i < jsonLength; i++) {
    Serial.print(payload[i], HEX);   // Ausgabe in Hexadezimal für bessere Lesbarkeit
    Serial.print(" ");
  }
}
