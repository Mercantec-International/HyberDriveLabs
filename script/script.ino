#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <WebSocketsClient.h>

// WiFi settings
char ssid[] = "Fibernet-IA01021636";            // Your network SSID (name)
char pass[] = "hjKwucMp";        // Your network password
int status = WL_IDLE_STATUS;     // WiFi connection status
WebSocketsClient webSocket;      // WebSocket client
unsigned long lastPingTime = 0;  // Timestamp for the last ping
const unsigned long pingInterval = 5000; // Ping interval in milliseconds

// Carrier object
MKRIoTCarrier carrier;
//Ultrasonic ultrasonic(A6);

// Variables for ultrasonic sensor and distance control
long lastLength = 0;  // Keeps track of the previous measured distance
unsigned long lastRequestTime = 0; // Timestamp for the last request

// Pin definitions for four motors
int enA = A1;   // Enable pin for motor A (PWM for speed)
int in1 = 2;    // In1 pin for motor A (direction)
int in2 = 3;    // In2 pin for motor A (direction)

int enB = A2;   // Enable pin for motor B (PWM for speed)
int in3 = 4;    // In1 pin for motor B (direction)
int in4 = 5;    // In2 pin for motor B (direction)

int enC = A5;   // Enable pin for motor C (PWM for speed)
int in5 = 14;   // In1 pin for motor C (direction)
int in6 = 13;   // In2 pin for motor C (direction)

int enD = A4;   // Enable pin for motor D (PWM for speed)
int in7 = 8;    // In1 pin for motor D (direction)
int in8 = 9;    // In2 pin for motor D (direction)

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize WiFi and Carrier
  initializeWiFi();
  initializeCarrier();
  printWiFiData();

  // Updated WebSocket configuration
  webSocket.beginSSL("hyperdrivelabs.onrender.com", 443, "/ws");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);
  
  // Add extra debug information
  Serial.println("WebSocket configuration complete");
  Serial.println("Waiting for messages...");
  
  carrier.leds.fill(0); // Fill all LEDs with black (RGB: 0, 0, 0)
  carrier.leds.show();  // Apply the changes

  

  // Initialize motor pins
  initializeMotorPins();

}

void loop() {
  // Read and print sensor data
  //readAndPrintSensorData();
  //delay(2000); // Wait 2 seconds before the next measurement
  
  webSocket.loop();
  
  // Send ping med jævne mellemrum
  unsigned long currentTime = millis();
  if (currentTime - lastPingTime > pingInterval) {
    webSocket.sendPing();
    lastPingTime = currentTime;
  }
}

// Function to initialize the WiFi connection
void initializeWiFi() {
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to network: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    delay(2000); // Wait 2 seconds to connect
  }
  Serial.println("Connected to network");
  Serial.println("----------------------------------------");
}

// Function to initialize the IoT Carrier
void initializeCarrier() {
  if (!carrier.begin()) {
    Serial.println("Carrier initialization failed!");
    while (1); // Block if the Carrier fails to initialize
  }
  delay(1000);
  Serial.println("Carrier initialized.");
  Serial.println("----------------------------------------");
}

// Function to print WiFi data
void printWiFiData() {
  Serial.println("WiFi data:");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("Signal Strength (RSSI): ");
  Serial.println(WiFi.RSSI());

  Serial.print("Encryption Type: ");
  Serial.println(WiFi.encryptionType(), HEX);
  Serial.println("----------------------------------------");
}

// Function to read, measure the distance, and print sensor data
void readAndPrintSensorData() {
  // Read temperature and humidity
  float temperature = carrier.Env.readTemperature();
  float humidity = carrier.Env.readHumidity();

  // Read distance
  //long distanceInCm = ultrasonic.read();

  Serial.println("Sensor Data:");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Distance: ");
  //Serial.print(distanceInCm);
  Serial.println(" cm");

  Serial.println("----------------------------------------");
}

// Function to initialize motor pins
void initializeMotorPins() {
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
}

// Function to set motor speeds and directions
void driveForward() {
  // Set motor speeds
  analogWrite(enA, 250); // Motor A
  analogWrite(enB, 250); // Motor B
  analogWrite(enC, 250); // Motor C
  analogWrite(enD, 250); // Motor D

  // Set motor directions
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);   // Motor A = FRONT RIGHT
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);   // Motor B = FRONT LEFT 
  digitalWrite(in5, HIGH); digitalWrite(in6, LOW);   // Motor C = BACK LEFT
  digitalWrite(in7, HIGH); digitalWrite(in8, LOW);   // Motor D = BACK RIGHT
}

// Function to stop all motors
void stopMotors() {
  // Set motor speeds to 0
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  analogWrite(enC, 0);
  analogWrite(enD, 0);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  // Tilføj generel event debugging
  Serial.print("[WSc] Event type: ");
  Serial.println(type);
  
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Afbrudt! Forsøger at genoprette forbindelse...");
      break;
      
    case WStype_CONNECTED:
      Serial.println("[WSc] Forbundet!");
      // Send en initial besked for at bekræfte forbindelsen
      webSocket.sendTXT("Arduino klar til kommandoer");
      break;
      
    case WStype_TEXT: {
      Serial.println("[WSc] TEXT EVENT MODTAGET!");
      // Debug payload som bytes
      Serial.print("[WSc] Payload bytes: ");
      for(size_t i=0; i<length; i++) {
        Serial.print(payload[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      // Debug payload som string
      String command = String((char *)payload);
      Serial.print("[WSc] Payload længde: ");
      Serial.println(length);
      Serial.print("[WSc] Rå payload: '");
      Serial.print(command);
      Serial.println("'");
      
      // Trim whitespace og konverter til lowercase for bedre sammenligning
      command.trim();
      command.toLowerCase();
      Serial.print("[WSc] Behandlet kommando: '");
      Serial.print(command);
      Serial.println("'");
      
      if (command == "forward") {
        Serial.println("[WSc] ✓ Matcher 'forward'");
        Serial.println("Udfører: Kør fremad");
        driveForward();
      }
      else if (command == "backward") {
        Serial.println("[WSc] ✓ Matcher 'backward'");
        Serial.println("Udfører: Bak");
      }
      else if (command == "stop") {
        Serial.println("[WSc] ✓ Matcher 'stop'");
        Serial.println("Udfører: Stop");
        stopMotors();
      } else {
        Serial.println("[WSc] ✗ Ingen match fundet");
        Serial.println("Kommando ikke genkendt!");
      }
      break;
    }
    
    case WStype_BIN:
      Serial.println("[WSc] Binær besked modtaget (ignoreret)");
      break;
      
    case WStype_FRAGMENT_TEXT_START:
      Serial.println("[WSc] Fragment text start modtaget");
      break;
      
    case WStype_FRAGMENT_BIN_START:
      Serial.println("[WSc] Fragment bin start modtaget");
      break;
      
    case WStype_FRAGMENT:
      Serial.println("[WSc] Fragment modtaget");
      break;
      
    case WStype_FRAGMENT_FIN:
      Serial.println("[WSc] Fragment fin modtaget");
      break;
      
    case WStype_PING:
      Serial.println("[WSc] Modtaget Ping");
      break;

    case WStype_PONG:
      Serial.println("[WSc] Modtaget Pong");
      break;
    
    case WStype_ERROR:
      Serial.println("[WSc] Fejl! Forsøger at genoprette forbindelse");
      break;
  }
}