import ms5837
from datetime import datetime
import requests
import smbus
import json
import time

# I2C multiplexer address
MUX_ADDR = 0x70

# Server URL for data sending
server_url = "http://127.0.0.1:5000/data"
bus = smbus.SMBus(1)


# Function to select multiplexer channel
def select_channel(channel):
    bus.write_byte(MUX_ADDR, 1 << channel)

# Function to read from a single MS5837 sensor
def read_sensor(sensor):
    try:
        if sensor.read():
            pressure = sensor.pressure()
            temperature = sensor.temperature()
            return pressure, temperature
    except Exception as e:
        print(f"Error reading sensor: {e}")
    return None

# Function to send data to the server
def send_data_to_server(data):
    headers = {'Content-Type': 'application/json'}
    try:
        response = requests.post(server_url, headers=headers, data=json.dumps(data))
        return response
    except requests.exceptions.RequestException as e:
        print(f"Error sending data to server: {e}")
        return None


# Initialize sensors
sensors = []
for i in range(8):
    select_channel(i)
    try:
        sensor = ms5837.MS5837_30BA()
        if sensor.init():
            sensors.append((i, sensor))
    except Exception as e:
        print(f"Error initializing sensor on channel {i}: {e}")

# Function to get data from the ESP32 (modify the URL according to your ESP32 endpoint)
def get_data_from_esp32():
    esp32_url = "http://192.168.178.31:5000/get_sensor_data"  # ESP32 URL for GET request
    try:
        response = requests.get(esp32_url)
        if response.status_code == 200:
            esp32_data = response.json()
            return esp32_data  # Return the JSON response from ESP32
        else:
            print(f"Error: Received {response.status_code} from ESP32")
            return None
    except requests.exceptions.RequestException as e:
        print(f"Error connecting to ESP32: {e}")
        return None

# need to modify the main4.py especially this function. so far it is working to get the values from esp32, but  not far till to read the values and display the stuffs

# Function to run the sensor reading loop
def run_sensor_reading():
    while True:
        sensor_data = {}

        # Read from local sensors connected via the multiplexer
        for channel, sensor in sensors:
            select_channel(channel)
            result = read_sensor(sensor)
            if result:
                pressure, temperature = result
                sensor_data[f"sensor{channel+1}"] = {
                    "pressure": round(pressure, 2),
                    "temperature": round(temperature, 2)
                }

                # Insert data into the database
                insert_sensor_data(f"sensor{channel+1}", temperature, pressure)

        # Fetch data from ESP32
        esp32_data = get_data_from_esp32()
        print(esp32_data)
        
        sensor_data['sensor0'] = esp32_data

        if sensor_data:
            print(sensor_data)
            send_data_to_server(sensor_data)

        time.sleep(1)  # Adjust delay if necessary