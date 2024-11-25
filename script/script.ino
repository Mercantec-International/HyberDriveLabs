#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <Ultrasonic.h>

// WiFi settings
char ssid[] = "Test";            // Your network SSID (name)
char pass[] = "nies1234";        // Your network password
int status = WL_IDLE_STATUS;     // WiFi connection status

// Carrier object
MKRIoTCarrier carrier;
Ultrasonic ultrasonic(A6);

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
  carrier.leds.fill(0); // Fill all LEDs with black (RGB: 0, 0, 0)
  carrier.leds.show();  // Apply the changes

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

  // Set motor directions
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);   // Motor A = FRONT RIGHT
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);   // Motor B = FRONT LEFT
  digitalWrite(in5, HIGH); digitalWrite(in6, LOW);   // Motor C = BACK LEFT
  digitalWrite(in7, HIGH); digitalWrite(in8, LOW);   // Motor D = BACK RIGHT
}

void loop() {
  // Read and print sensor data
  readAndPrintSensorData();
  delay(2000); // Wait 2 seconds before the next measurement
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
  long distanceInCm = ultrasonic.read();

  Serial.println("Sensor Data:");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Distance: ");
  Serial.print(distanceInCm);
  Serial.println(" cm");

  Serial.println("----------------------------------------");
}
