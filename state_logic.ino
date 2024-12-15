#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>  // Replace with your specific accelerometer library

// Pin definitions
const int buttonPin = 2;        // Pin connected to the button
const int buzzerPin = 9;        // Buzzer pin
const int ledPin = 13;          // LED pin

// State definitions
enum LockState {
  LOCKED,
  UNLOCKED,
  MOVEMENT
};

LockState currentState = LOCKED; // Start in LOCKED state

// Setup the accelerometer (adjust this if you use a different sensor)
Adafruit_LSM303_Accel accel;

// Setup
void setup() {
  Serial.begin(9600);

  // Setup button pin
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Initialize accelerometer
  if (!accel.begin()) {
    Serial.println("Couldn't find the sensor");
    while (1);
  }

  update_lock_state();  // Update the lock state on startup
}

// Main loop
void loop() {
  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    toggle_lock_state();  // Toggle between LOCKED and UNLOCKED
    delay(500);  // Debounce delay for button press
  }

  // Check if accelerometer detects abnormal movement
  sensors_event_t event;
  accel.getEvent(&event);

  // If movement is detected (simplified check, you may want to add thresholds)
  if (abs(event.acceleration.x) > 2.0 || abs(event.acceleration.y) > 2.0 || abs(event.acceleration.z) > 2.0) {
    if (currentState == LOCKED) {
      currentState = MOVEMENT;  // Transition to MOVEMENT if locked
      send_alert("MOVEMENT");   // Send MQTT alert for movement
    }
  }

  // Update the state of the lock system
  update_lock_state();
  delay(100);  // Slow down the loop to avoid excessive reads
}

// Toggle between LOCKED and UNLOCKED states when the button is pressed
void toggle_lock_state() {
  if (currentState == LOCKED) {
    currentState = UNLOCKED;
    send_alert("UNLOCKED");    // Send MQTT alert for unlocked state
  } else {
    currentState = LOCKED;
    send_alert("LOCKED");      // Send MQTT alert for locked state
  }
}

// Update the lock system's state (LED and Buzzer)
void update_lock_state() {
  switch (currentState) {
    case LOCKED:
      digitalWrite(ledPin, HIGH);   // LED on when locked
      digitalWrite(buzzerPin, LOW); // Buzzer off when locked
      Serial.println("LOCKED");
      break;
    case UNLOCKED:
      digitalWrite(ledPin, LOW);    // LED off when unlocked
      digitalWrite(buzzerPin, LOW); // Buzzer off when unlocked
      Serial.println("UNLOCKED");
      break;
    case MOVEMENT:
      digitalWrite(ledPin, LOW);    // LED off during movement
      digitalWrite(buzzerPin, HIGH); // Buzzer on during movement
      Serial.println("MOVEMENT");
      break;
  }
}

// Function to send an alert via MQTT (to be implemented)
void send_alert(String message) {
  // Add your MQTT publishing logic here (communicate with the ESP8266)
  Serial.print("Sending alert: ");
  Serial.println(message);
}
