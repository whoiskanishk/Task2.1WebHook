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

// Define acceptable range for temperature and humidity
const float TEMP_MIN = 18.0;
const float TEMP_MAX = 28.0;
const float HUM_MIN = 30.0;
const float HUM_MAX = 60.0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000); // wait 5 seconds before retrying
  }
  Serial.println("\nConnected to WiFi.");
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

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

  // Update ThingSpeak fields with temperature and humidity
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  // Check if temperature or humidity are outside the acceptable range
  bool alarm = false;
  if (temperature < TEMP_MIN || temperature > TEMP_MAX || humidity < HUM_MIN || humidity > HUM_MAX) {
    alarm = true; // Set alarm flag
    Serial.println("Alarm: Sensor readings are outside the acceptable range!");
  }

  // Use Field3 to report the alarm status (1 for alarm, 0 for normal)
  ThingSpeak.setField(3, alarm ? 1 : 0);
  
  // Write the fields to ThingSpeak
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  delay(60000); // Wait 60 seconds before sending new data
}
