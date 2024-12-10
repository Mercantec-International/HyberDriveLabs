#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <Ultrasonic.h>
#include <WebSocketsClient.h>

#define WIFI_SSID "MAGS-OLC"            // Your network SSID (name)
#define WIFI_PASS "Merc1234!"        // Your network password

int status = WL_IDLE_STATUS;     // WiFi connection status
WiFiClient client;
WebSocketsClient webSocket;

MKRIoTCarrier carrier;
Ultrasonic ultrasonic(A6);  // Initialize ultrasonic sensor on pin A6

// Pin definitions for four motors
int enA = A5;   // Enable pin for motor A (PWM for speed)
int in1 = 9;    // In1 pin for motor A (direction)
int in2 = 6;    // In2 pin for motor A (direction)

int enB = A6;   // Enable pin for motor B (PWM for speed)
int in3 = 8;    // In1 pin for motor B (direction)
int in4 = 7;    // In2 pin for motor B (direction)

int enC = A1;   // Enable pin for motor C (PWM for speed)
int in5 = 4;   // In1 pin for motor C (direction)
int in6 = 3;   // In2 pin for motor C (direction)

int enD = A2;   // Enable pin for motor D (PWM for speed)
int in7 = 1;    // In1 pin for motor D (direction)
int in8 = 5;    // In2 pin for motor D (direction)

unsigned long lastPingTime = 0;
const unsigned long pingInterval = 5000;

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Initialize WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to network: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(5000);
  }
  Serial.println("Connected to WiFi");

  // Initialize WebSocket
  webSocket.beginSSL("car.mercantec.tech", 443, "/ws");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);

  // Initialize motor pins
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(enC, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);

  pinMode(enD, OUTPUT);
  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);

  // Set motor speeds
  analogWrite(enA, 250); // Motor A
  analogWrite(enB, 250); // Motor B
  analogWrite(enC, 250); // Motor C
  analogWrite(enD, 250); // Motor D

  carrier.leds.setBrightness(0); // Helderheid op 0%
  carrier.leds.fill(0);          // Alle LED's op zwart (uit)
  carrier.leds.show();           // Update de LED-status

  Serial.println("WebSocket configuration complete");
  Serial.println("Waiting for messages...");
}

void loop() {
  webSocket.loop();  // Maintain WebSocket connection
  
  

  // Send ping periodically
  unsigned long currentTime = millis();
  if (currentTime - lastPingTime > pingInterval) {
    webSocket.sendPing();
    lastPingTime = currentTime;
  }
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  // Add general event debugging
  Serial.print("[WSc] Event type: ");
  Serial.println(type);

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected! Trying to reconnect...");
      break;
      
    case WStype_CONNECTED:
      Serial.println("[WSc] Connected!");
      // Send an initial message to confirm the connection
      webSocket.sendTXT("Arduino ready for commands");
      break;

    case WStype_TEXT: {
      String command = String((char *)payload);
      command.trim();
      command.toLowerCase();

      Serial.print("[WSc] Command received: ");
      Serial.println(command);

      if (command == "forward") {
        moveForward();
      }
      else if (command == "backward") {
        moveBackward();
      }
      else if (command == "stop") {
        stopMotors();
      } else {
        Serial.println("[WSc] Command not recognized!");
      }
      break;
    }
    
    case WStype_BIN:
      Serial.println("[WSc] Binary message received (ignored)");
      break;
      
    case WStype_ERROR:
      Serial.println("[WSc] Error! Trying to reconnect");
      break;
  }
}

// Move motors forward
void moveForward() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);   // Motor A = FRONT RIGHT
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);   // Motor B = FRONT LEFT
  digitalWrite(in5, HIGH); digitalWrite(in6, LOW);   // Motor C = BACK LEFT
  digitalWrite(in7, LOW); digitalWrite(in8, HIGH);   // Motor D = BACK RIGHT
  Serial.println("Moving forward");
}

// Move motors backward
void moveBackward() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);   // Motor A = BACK RIGHT
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);   // Motor B = BACK LEFT
  digitalWrite(in5, LOW); digitalWrite(in6, HIGH);   // Motor C = FRONT LEFT
  digitalWrite(in7, HIGH); digitalWrite(in8, LOW);   // Motor D = FRONT RIGHT
  Serial.println("Moving backward");
}

// Stop motors
void stopMotors() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);   // Motor A = STOP
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);   // Motor B = STOP
  digitalWrite(in5, LOW); digitalWrite(in6, LOW);   // Motor C = STOP
  digitalWrite(in7, LOW); digitalWrite(in8, LOW);   // Motor D = STOP
  Serial.println("Motors stopped");
}
