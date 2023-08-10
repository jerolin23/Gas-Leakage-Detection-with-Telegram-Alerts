
#include <Ethernet.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "HardwareSerial.h"

const char* device_unique_id = "GasLeakageConcentration_" __DATE__ "_" __TIME__;

#define SERIAL_OUTPUT
#ifdef SERIAL_OUTPUT
#define PRINT(x)   Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#else
#define PRINT(x)   
#define PRINTLN(x) 
#endif

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 30, 0xF7
};
IPAddress ip(192, 168, 0, 77);
//IPAddress ip(10, 5, 15, 109);

// Enter the IP address of the server you're connecting to:
//IPAddress server(192, 168, 100, 2);
//IPAddress server(10, 5, 15, 78);
IPAddress server(10, 21, 70, 16);
IPAddress myDns(192, 168, 0, 1);

IPAddress mqtt_server(10, 21, 70, 16);
//IPAddress mqtt_server(44, 195, 202, 69);
EthernetClient mqttClient;
PubSubClient mqtt_client(mqttClient);
// bool capture_requested = 0;
const int Sensor1Pin = A0; 
const int Sensor2Pin = A1;   
const int Sensor3Pin = A2;   
const int Sensor4Pin = A3;

// Publish the sensor values to their respective MQTT topics
const char Sensor1Topic[] = "GS-1";
const char Sensor2Topic[] = "GS-2";
const char Sensor3Topic[] = "GS-3";
const char Sensor4Topic[] = "GS-4";

//===================================================================================================

// void callback(char* topic, byte* payload, unsigned int length) {
//   PRINT(">>>>>>>>>>> Message arrived [");
//   PRINT(topic);
//   PRINT("] ");
//   String cmd = "";
//   for (int i=0;i<length;i++) {
//     cmd += (char)payload[i];
//     PRINT((char)payload[i]);
//   }
//   PRINTLN();

//   if (cmd == "cmd:capture") {
//     capture_requested = 1;
//     PRINTLN("Capture requested!!!");
//   }
// }

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    PRINT("MQTT: Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt_client.connect(device_unique_id)) {
      PRINTLN("MQTT: connected");
      // ... and resubscribe
      mqtt_client.subscribe("MQTT-W5300");
    } else {
      PRINT("MQTT: failed, rc=");
      PRINT(mqtt_client.state());
      PRINTLN(" MQTT: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

//===================================================================================================

void setup() {
    // Open serial communications and wait for port to open:
  Serial3.setRx(PC11);
  Serial3.setTx(PC10);  
  delay(50);

    // Serial.println("pinMode...");
  pinMode(Sensor1Pin, INPUT);  
  pinMode(Sensor2Pin, INPUT);    
  pinMode(Sensor3Pin, INPUT); 
  pinMode(Sensor4Pin, INPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
   // start the Ethernet connection:
  PRINTLN("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    PRINTLN("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      PRINTLN("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      PRINTLN("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    PRINT("  DHCP assigned IP ");
    PRINTLN(Ethernet.localIP());
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(callback);

  // give the Ethernet shield a second to initialize:
  delay(1000);

}

String valstr = "";

void loop() {

  if (!mqtt_client.connected()) {
    reconnect();
  }
  mqtt_client.loop();

  // Read sensor values
  int Sensor1Value = readSensor1();
  int Sensor2Value = readSensor2();
  int Sensor3Value = readSensor3();
  int Sensor4Value = readSensor4();

  valstr = "";
  valstr += Sensor1Value;
  PRINTLN(valstr);
  mqtt_client.publish(Sensor1Topic, valstr.c_str());

  valstr = "";
  valstr += Sensor2Value;
  PRINTLN(valstr);
  mqtt_client.publish(Sensor2Topic, valstr.c_str());

  valstr = "";
  valstr += Sensor3Value;
  PRINTLN(valstr);
  mqtt_client.publish(Sensor3Topic, valstr.c_str());

  valstr = "";
  valstr += Sensor4Value;
  PRINTLN(valstr);
  mqtt_client.publish(Sensor4Topic, valstr.c_str());

  // Adjust the interval between readings and publications
  delay(3000);
}

// Function to read gas sensor value
int readSensor1() {
  // Read the analog value from the gas sensor using analogRead function
  int Sensor1Value = analogRead(Sensor1Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 1 Value: ");
  Serial.println(Sensor1Value);

  return Sensor1Value; // Return the raw analog value without mapping or scaling
}

// Function to read sensor 2 value
int readSensor2() {
  // Read the analog value from the second sensor using analogRead function
  int Sensor2Value = analogRead(Sensor2Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 2 Value: ");
  Serial.println(Sensor2Value);

  return Sensor2Value; // Return the raw analog value without mapping or scaling
}

// Function to read sensor 3 value
int readSensor3() {
  // Read the analog value from the third sensor using analogRead function
  int Sensor3Value = analogRead(Sensor3Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 3 Value: ");
  Serial.println(Sensor3Value);

  return Sensor3Value; // Return the raw analog value without mapping or scaling
}

// Function to read sensor 4 value
int readSensor4() {
  // Read the analog value from the third sensor using analogRead function
  int Sensor4Value = analogRead(Sensor4Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 4 Value: ");
  Serial.println(Sensor4Value);

  return Sensor4Value; // Return the raw analog value without mapping or scaling
}
