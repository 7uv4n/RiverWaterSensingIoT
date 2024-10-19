#include <WiFi.h>
#include <WebServer.h>
#include <MS5837.h>
#include <OneWire.h>
#include <DallasTemperature.h>

MS5837 sensor;

// Wi-Fi credentials
const char* ssid = "HotspotElliot";
const char* password = "mrrobotjohnson";

// Flask server IP (your local IP)
WebServer server(5000);  // Web server running on port 5000

// DS18B20 setup
#define ONE_WIRE_BUS 4  // Pin where the DS18B20 is connected
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.println(WiFi.localIP()); // Print ESP32 IP address

  // Initialize I2C for MS5837 sensor
  Wire.begin(23, 22);  // SDA, SCL

  // Initialize the MS5837 sensor
  if (!sensor.init()) {
    Serial.println("Failed to initialize the MS5837 sensor!");
    while (1); // Halt the program if sensor initialization fails
  }

  // Set sensor model
  sensor.setModel(MS5837::MS5837_30BA);

  // Set fluid density (997 for fresh water, 1029 for seawater)
  sensor.setFluidDensity(997);

  // Initialize DS18B20
  ds18b20.begin();

  // Set up the web server to handle GET requests
  server.on("/get_sensor_data", HTTP_GET, handleGetSensorData);
  server.begin();
  Serial.println("Server started and ready to receive GET requests");
}

void loop() {
  // Continuously handle incoming HTTP requests
  server.handleClient();
}

// Function to handle GET requests and return sensor data as JSON
void handleGetSensorData() {
  sensor.read(); // Update MS5837 sensor readings
  ds18b20.requestTemperatures();  // Request DS18B20 temperature

  // Prepare sensor data as JSON
  String jsonResponse = "{";
  jsonResponse += "\"temperature_ms5837\": " + String(sensor.temperature()) + ",";
  jsonResponse += "\"pressure_ms5837\": " + String(sensor.pressure()) + ",";
  jsonResponse += "\"temperature_ds18b20\": " + String(ds18b20.getTempCByIndex(0));
  jsonResponse += "}";

  // Send the JSON response
  server.send(200, "application/json", jsonResponse);
}
