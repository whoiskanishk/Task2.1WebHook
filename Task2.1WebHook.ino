#include <SPI.h>
#include <WiFiNINA.h>
#include <DHT.h>
#include <ThingSpeak.h>

// Replace with your network credentials
const char* ssid = "ddh";
const char* password = "123456789";

// ThingSpeak Settings
unsigned long myChannelNumber = 2501014;
const char* myWriteAPIKey = "EU2TP8BUHSCZM54W";

WiFiClient client;

#define DHTPIN 2          // Pin which the DHT sensor is connected to
#define DHTTYPE DHT11     // DHT 11 (Change to DHT22 if using that model)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize serial communication and DHT sensor.
  Serial.begin(9600);
  dht.begin();

  // Attempt to connect to WiFi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    // Retry until connected
    Serial.print(".");
    delay(5000); // wait 5 seconds before retrying
  }
  Serial.println("\nConnected to WiFi.");

  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  // Read humidity and temperature values
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");

  // Write to ThingSpeak. Field1 for temperature, Field2 for humidity.
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  // Write the fields that you've set all at once.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  // Wait 60 seconds to update the data again
  delay(60000);
}
