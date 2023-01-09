#include<PubSubClient.h>
#include<WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTTYPE DHT11
/*
 * WLAN Configuration
*/
const char* ssid = "Sharvin's Galaxy S10"; // FILL THIS ACCORDINGLY
const char* password = "nakk7194"; // FILL THIS ACCORDINGLY

/*
 * MQTT Broker configuration + Topic
*/
const char* mqtt_broker = "0.tcp.ap.ngrok.io"; // CHANGE THIS ACCORDINGLY
const char* topic = "test/1/env"; // CHANGE SensorID here!


WiFiClient espClient;
PubSubClient client(espClient);

const int dht11Pin = 22;

//input sensor

DHT dht(dht11Pin, DHTTYPE);

/*
 * JSON Data Format
*/
const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
DynamicJsonDocument doc(capacity);
JsonObject data = doc.createNestedObject("data");

float temp, humid = 0.0;
char output[128];


/*
 * reconnect: connect/reconnect to MQTT broker
*/

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
       delay(500);
       Serial.print(".");
  }
  Serial.println("WiFi Connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  client.setServer(mqtt_broker, 16059);
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Creating JSON Format
  data["temp"].set(dht.readTemperature());
  data["humid"].set(dht.readHumidity());

  // Output Format
  serializeJson(doc, output);
  
  Serial.println(output); // Data Format 
  client.publish(topic, output);
  delay(2000); // Publish data every 2 seconds to the Broker
}