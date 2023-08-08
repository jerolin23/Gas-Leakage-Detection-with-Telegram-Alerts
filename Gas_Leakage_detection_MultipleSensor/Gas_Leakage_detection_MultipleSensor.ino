#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 30, 0xFE, 0xED };
IPAddress ip(172, 16, 0, 100);
IPAddress server(44, 195, 202, 69);


const int Sensor1Pin = 35; 
const int Sensor2Pin = 34;   
const int Sensor3Pin = 32;   
const int Sensor4Pin = 31;

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient30")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Out", "hello world");
      // ... and resubscribe
      client.subscribe("gas_concentration");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 10 seconds");
      // Wait 5 seconds before retrying
      delay(10000);
    }
  }
}

void setup()
{
  Serial.begin(57600);
  Ethernet.init(17);
  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac);
  delay(1500);

  pinMode(Sensor1Pin, INPUT);  
  pinMode(Sensor2Pin, INPUT);    
  pinMode(Sensor3Pin, INPUT); 
  pinMode(Sensor4Pin, INPUT);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor values
  float Sensor1Value = readSensor1();
  float Sensor2Value = readSensor2();
  float Sensor3Value = readSensor3();
  float Sensor4Value = readSensor4();

  // Publish the sensor values to their respective MQTT topics
  char Sensor1Topic[] = "GS-1";
  char Sensor2Topic[] = "GS-2";
  char Sensor3Topic[] = "GS-3";
  char Sensor4Topic[] = "GS-4";

  char payload[10];

  snprintf(payload, sizeof(payload), "%f", Sensor1Value);
  client.publish(Sensor1Topic, payload);

  snprintf(payload, sizeof(payload), "%f", Sensor2Value);
  client.publish(Sensor2Topic, payload);

  snprintf(payload, sizeof(payload), "%f", Sensor3Value);
  client.publish(Sensor3Topic, payload);

  snprintf(payload, sizeof(payload), "%f", Sensor4Value);
  client.publish(Sensor4Topic, payload);

  // Adjust the interval between readings and publications
  delay(10000);
}

// Function to read gas sensor value
float readSensor1() {
  // Read the analog value from the gas sensor using analogRead function
  int Sensor1Value = analogRead(Sensor1Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 1 Value: ");
  Serial.println(Sensor1Value);

  return Sensor1Value; // Return the raw analog value without mapping or scaling
}

// Function to read sensor 2 value
float readSensor2() {
  // Read the analog value from the second sensor using analogRead function
  int Sensor2Value = analogRead(Sensor2Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 2 Value: ");
  Serial.println(Sensor2Value);

  return Sensor2Value; // Return the raw analog value without mapping or scaling
}

// Function to read sensor 3 value
float readSensor3() {
  // Read the analog value from the third sensor using analogRead function
  int Sensor3Value = analogRead(Sensor3Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 3 Value: ");
  Serial.println(Sensor3Value);

  return Sensor3Value; // Return the raw analog value without mapping or scaling
}

// Function to read sensor 4 value
float readSensor4() {
  // Read the analog value from the third sensor using analogRead function
  int Sensor4Value = analogRead(Sensor4Pin);

  // Display the raw analog value on the Serial Monitor
  Serial.print("Sensor 4 Value: ");
  Serial.println(Sensor4Value);

  return Sensor4Value; // Return the raw analog value without mapping or scaling
}
