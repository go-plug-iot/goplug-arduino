#include<PubSubClient.h>
#include<WiFi.h>
#include <ArduinoJson.h>
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

long lastMsg = 0;
char msg[50];
int value = 0;

const int ledPin1 = 23;
const int ledPin2 = 22;
/*
 * JSON Data Format
*/
const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
DynamicJsonDocument doc(capacity);
JsonObject data = doc.createNestedObject("data");

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
      client.subscribe("SWITCH_ON");
      client.subscribe("SWITCH_OFF");
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
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  client.setServer(mqtt_broker, 17340);
  client.setCallback(callback);
  client.subscribe("SWITCH_ON");
  client.subscribe("SWITCH_OFF");
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);
  DynamicJsonDocument root(1024);
  deserializeJson(root, messageTemp);

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "SWITCH_ON") {
    if(root["switchId"] == "1"){
      Serial.println("on");
      digitalWrite(ledPin1, HIGH);
    }
    else if(root["switchId"]  == "2"){
      Serial.println("on");
      digitalWrite(ledPin2, HIGH);
    }
  }
  else if (String(topic) == "SWITCH_OFF") {
    if(root["switchId"]  == "1"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }
    else if(root["switchId"]  == "2"){
      Serial.println("off");
      digitalWrite(ledPin2, LOW);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Creating JSON Format
  data["isOn"].set(digitalRead(ledPin1));
  data["voltage"].set("232323");
  data["current"].set("232324");
  data["power"].set("232325");

  // Output Format
  serializeJson(doc, output);
  
  //Serial.println(output); // Data Format 
  client.publish("SWITCH_1", output);


  data["isOn"].set(digitalRead(ledPin2));
  data["voltage"].set("332323");
  data["current"].set("254352324");
  data["power"].set("232325354");
 // Output Format
  serializeJson(doc, output);
  
  //Serial.println(output); // Data Format 
  client.publish("SWITCH_2", output);

  delay(2000); // Publish data every 2 seconds to the Broker
}