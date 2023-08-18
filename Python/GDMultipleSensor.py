import paho.mqtt.client as mqtt
import logging
import requests
import time
from influxdb_client import  Point, WritePrecision
from influxdb_client.client import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
from influxdb_client import InfluxDBClient


# Set up logging to see any potential errors
logging.basicConfig(level=logging.INFO)

# MQTT broker settings
mqtt_broker_url = 'broker.emqx.io'  # Replace this with your MQTT broker's domain name or IP address
mqtt_port = 1883
mqtt_keepalive = 60

# Callback function for when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
        for sensor in gas_sensors:
            client.subscribe(sensor["topic"])  # Subscribe to the desired MQTT topic for each gas sensor
    else:
        print(f"Connection failed with code {rc}")

# Function to send Telegram notification
def send_telegram_notification(message):
    telegram_bot_token = '6535063345:*******************'
    telegram_chat_id = '1063********'

    telegram_api_url = f"https://api.telegram.org/bot{telegram_bot_token}/sendMessage"
    data = {
        "chat_id": telegram_chat_id,
        "text": message,
    }
    try:
        response = requests.post(telegram_api_url, data=data)
        response.raise_for_status()  # Raise an exception if the response status is not 2xx (indicating success)
        print("Message sent to Telegram successfully")
    except requests.exceptions.RequestException as e:
        print("Error sending message to Telegram:", e)

def on_message(client, userdata, msg):
    global send_mqtt_messages

    payload = msg.payload.decode('utf-8')
    print("Received MQTT message:", payload)

    # Find the corresponding sensor based on the topic
    for sensor in gas_sensors:
        if msg.topic == sensor["topic"]:
            # Write the received value to InfluxDB with the unique field name
            field_name = "gas_concentration_" + sensor["name"]  # Generate unique field name for each sensor
            ts.write_data_to_influxdb(sensor["measure"], field_name, float(payload))
            # Call the function to perform some action after writing data to InfluxDB
            ts.after_writing_to_influxdb()
            # Set the flag to True after writing to InfluxDB
            send_mqtt_messages = True
            break

# Function to process data for a specific gas sensor
def process_gas_sensor(sensor):
    # Calculate the average of the received values
    if len(sensor["mqtt_values"]) > 0:
        average = sum(sensor["mqtt_values"]) / len(sensor["mqtt_values"])

        field_name = "gas_concentration_" + sensor["name"]  # Generate unique field name for each sensor

        if sensor['name'] == 'Sensor1' and average > 9:
            send_telegram_notification(f"{sensor['name']} - Average value: {average:.2f} LEAKAGE DETECTED!!!")
        elif sensor['name'] == 'Sensor2' and average > 1000:
            send_telegram_notification(f"{sensor['name']} - Average value: {average:.2f} LEAKAGE DETECTED!!!")
        elif sensor['name'] == 'Sensor3' and average > 1000:
            send_telegram_notification(f"{sensor['name']} - Average value: {average:.2f} LEAKAGE DETECTED!!!")
        elif sensor['name'] == 'Sensor4' and average > 1000:
            send_telegram_notification(f"{sensor['name']} - Average value: {average:.2f} LEAKAGE DETECTED!!!")
        else:
            print(f"{sensor['name']} - No Leakage")

        print(f"{sensor['name']} - Average value: {average:.2f}")

        # Write the average value to InfluxDB with the unique field name
        ts.write_data_to_influxdb(sensor["measure"], field_name, average)

    else:
        print(f"{sensor['name']} - No Leakage")

    # Clear the list for the next interval
    sensor["mqtt_values"].clear()


# Create MQTT client
mqtt_client = mqtt.Client()

# Set callback functions
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

# Connect to MQTT broker
mqtt_client.connect(mqtt_broker_url, mqtt_port, mqtt_keepalive)

# Start MQTT loop in the background
mqtt_client.loop_start()

class InfluxDB:
    def __init__(self, bucket="AIoT"):
        self.token = "4S-CyOmTPUhSgUiUSf0A8Fn31keTgErGXCoxoN1makt745rOx************************"
        self.org = "Christ"
        # self.url = "http://localhost:8086"
        self.url = "http://10.21.70.16:8086"
        self.bucket = bucket
        self.client = influxdb_client.InfluxDBClient(url=self.url, token=self.token, org=self.org)
        self.write_api = None
        self.delete_api = None
        self.query_api = None

    def delete(self, measure, start, stop):
        if self.delete_api is None:
            self.delete_api = self.client.delete_api()

        # start = "1970-01-01T00:00:00Z"
        # stop = "2100-01-01T00:00:00Z"
        return self.delete_api.delete(start, stop, "_measurement=" + measure, bucket=self.bucket, org=self.org)

    def write_data_to_influxdb(self, measure, field_name, value):
        # Write the received gas concentration data to InfluxDB
        if self.write_api is None:
            self.write_api = self.client.write_api(write_options=SYNCHRONOUS)

        point = (Point(measure).field(field_name, value))

        try:
            self.write_api.write(bucket=self.bucket, org=self.org, record=point)
            print("Data point written to InfluxDB:", point.to_line_protocol())
        except Exception as e:
            print("Error writing to InfluxDB:", e)
    def after_writing_to_influxdb(self):
        print("Data written to InfluxDB. Perform desired action here.")

    def query(self, measure, start):
        if self.query_api is None:
            self.query_api = self.client.query_api()

        query = f"""from(bucket: "{self.bucket}")
         |> range(start: {start})
         |> filter(fn: (r) => r._measurement == "{measure}")"""
        tables = self.query_api.query(query, org=self.org)

        return tables

    def query_mean(self, measure, start):
        if self.query_api is None:
            self.query_api = self.client.query_api()

        query = f"""from(bucket: "{self.bucket}")
         |> range(start: {start})
         |> filter(fn: (r) => r._measurement == "{measure}")
         |> mean()"""
        tables = self.query_api.query(query, org=self.org)

        return tables
# Define gas sensors and their settings
gas_sensors = [
    {
        "name": "Sensor1",
        "measure": "MQ-7",
        "topic": "GS-1",
        "mqtt_values": []
    },
    {
        "name": "Sensor2",
        "measure": "MQ-9",
        "topic": "GS-2",
        "mqtt_values": []
    },
    {
        "name": "Sensor3",
        "measure": "MQ-3",
        "topic": "GS-3",
        "mqtt_values": []
    },
    {
        "name": "Sensor4",
        "measure": "MQ-4",
        "topic": "GS-4",
        "mqtt_values": []
    }

]

if __name__ == '__main__':
    ts = InfluxDB(bucket="AIoT")

    try:
        while True:
            # Set the flag to False at the start of each iteration
            send_mqtt_messages = False

            # Query data for the last 2 minutes for each sensor
            for sensor in gas_sensors:
                tables = ts.query_mean(sensor["measure"], "-2m")

                # Calculate the average of query results for each sensor
                for table in tables:
                    for record in table.records:
                        value = record.get_value()
                        sensor["mqtt_values"].append(value)

                # Process data for the current sensor
                process_gas_sensor(sensor)

            # Wait for 2 minutes before calculating the next average
            time.sleep(120)

    except KeyboardInterrupt:
        # This block will execute when Ctrl+F2 is pressed
        print("Script terminated by user.")
        # Optionally, you can send a final message to Telegram notifying the script termination
        send_telegram_notification("Script terminated by user.")





