import requests
import json
import time
import random

# Server URL for sending data
# server_url = "http://127.0.0.1:5000/data"
server_url = "https://riverwatersensingiot.azurewebsites.net/data"
validator_value = "YuvanIsAwesome"

# Function to generate random sensor data
def generate_sensor_data():
    sensor_data = {}
    for sensor_id in range(0, 5):  # Simulate 4 sensors
        temperature = round(random.uniform(20.0, 30.0), 2)
        pressure = round(random.uniform(900.0, 1100.0), 2)
        sensor_data["Validator"] = validator_value
        sensor_data[f"sensor{sensor_id}"] = {
            "temperature": temperature,
            "pressure": pressure
        }
    return sensor_data

# Function to send the sensor data to the Flask server
def send_data_to_server(data):
    headers = {'Content-Type': 'application/json'}
    try:
        response = requests.post(server_url, headers=headers, data=json.dumps(data))
        if response.status_code == 200:
            print(f"Data sent successfully: {data}")
        else:
            print(f"Failed to send data. Status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Error sending data: {e}")

if __name__ == "__main__":
    while True:
        sensor_data = generate_sensor_data()
        send_data_to_server(sensor_data)
        time.sleep(5)  # Send data every 5 seconds
