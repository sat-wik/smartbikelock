#include <ESP8266WiFi.h>        // ESP8266 Wi-Fi library
#include <PubSubClient.h>        // MQTT Library

// Wi-Fi and MQTT settings
const char* ssid = "your_wifi_ssid";            // Replace with your Wi-Fi SSID
const char* password = "your_wifi_password";    // Replace with your Wi-Fi password
const char* mqtt_server = "broker.hivemq.com";  // MQTT broker address

WiFiClient espClient;
PubSubClient client(espClient);

const char* lockStatusTopic = "bike/esp8266/smartlock/lock_status";
const char* tamperAlertTopic = "bike/esp8266/smartlock/tamper_alert";
const char* movementAlertTopic = "bike/esp8266/smartlock/movement_alert";

// Setup function
void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);

  // Connect to MQTT broker
  while (!client.connected()) {
    if (client.connect("bike_lock_esp8266")) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe(lockStatusTopic);
      client.subscribe(tamperAlertTopic);
      client.subscribe(movementAlertTopic);
    } else {
      Serial.print("Failed to connect to MQTT broker. Trying again...");
      delay(5000);
    }
  }
}

// Wi-Fi connection setup
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
  Serial.println("IP Address: " + WiFi.localIP().toString());
}

// Callback function when a message is received
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println("Received message: " + message + " on topic: " + String(topic));
}

// Function to publish lock status
void publish_lock_status(const char* status) {
  Serial.println("Sending lock status: " + String(status));
  client.publish(lockStatusTopic, status);
}

// Function to send tamper alert
void publish_tamper_alert(const char* alert) {
  Serial.println("Sending tamper alert: " + String(alert));
  client.publish(tamperAlertTopic, alert);
}

// Function to send movement alert
void publish_movement_alert(const char* alert) {
  Serial.println("Sending movement alert: " + String(alert));
  client.publish(movementAlertTopic, alert);
}

// Main loop
void loop() {
  client.loop();  // Maintain MQTT connection
  
  // Check for incoming data from Arduino via serial (i.e., state changes or sensor readings)
  if (Serial.available() > 0) {
    String input = Serial.readString();
    
    if (input == "LOCKED") {
      publish_lock_status("LOCKED");
    }
    else if (input == "UNLOCKED") {
      publish_lock_status("UNLOCKED");
    }
    else if (input == "TAMPERED") {
      publish_tamper_alert("Mesh cut detected!");
    }
    else if (input == "MOVEMENT") {
      publish_movement_alert("Strange shaking detected!");
    }
  }
}
