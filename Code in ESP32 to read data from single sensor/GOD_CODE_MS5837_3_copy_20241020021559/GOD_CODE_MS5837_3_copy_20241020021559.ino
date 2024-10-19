#include <WiFi.h>
#include <WebServer.h>
#include <MS5837.h>

MS5837 sensor;

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// Flask server IP (your local IP)
WebServer server(5000);  // Web server running on port 5000

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

  // Initialize the sensor
  if (!sensor.init()) {
    Serial.println("Failed to initialize the sensor!");
    while (1); // Halt the program if sensor initialization fails
  }

  // Set sensor model
  sensor.setModel(MS5837::MS5837_30BA);

  // Set fluid density (997 for fresh water, 1029 for seawater)
  sensor.setFluidDensity(997);

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
  sensor.read(); // Update sensor readings

  // Prepare sensor data as JSON
  String jsonResponse = "{";
  jsonResponse += "\"temperature\": " + String(sensor.temperature()) + ",";
  jsonResponse += "\"pressure\": " + String(sensor.pressure());
  jsonResponse += "}";

  // Send the JSON response
  server.send(200, "application/json", jsonResponse);
}
