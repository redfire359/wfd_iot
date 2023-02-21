#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// GPIO 5 D1
#define pin1 13
#define pin2 5
#define testpin 2

// WiFi
const char *ssid = "hydro"; // Enter your WiFi name
const char *password = "hydrohydro";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.1.179";
const char *topic = "grit/pumpout";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(testpin, OUTPUT);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
    // Set software serial baud to 115200;
    Serial.begin(115200);
    // connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        const char *client_id = "esp8266-client-";
        Serial.println("Connecting to public emqx mqtt broker.....");
        if (client.connect(client_id)) {
            Serial.println("Public emqx mqtt broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // publish and subscribe
    client.publish(topic, "hello emqx");
    client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char) payload[i];  // convert *byte to string
    }
    Serial.print(message);
    if (message == "in") { 
      digitalWrite(pin1, HIGH); 
      digitalWrite(pin2, LOW); 
      digitalWrite(testpin, LOW);
    }   // LED on
    if (message == "out") {
      digitalWrite(pin1, LOW); 
      digitalWrite(pin2, HIGH);
      digitalWrite(testpin, LOW); 
    } // LED off
    if (message == "off") {
      digitalWrite(pin1, HIGH); 
      digitalWrite(pin2, HIGH);
      digitalWrite(testpin, HIGH); 
    } // LED off
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    client.loop();
}
