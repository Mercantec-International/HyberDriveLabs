#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <WebSocketsClient.h>

#define WIFI_SSID "GalaxyS22"
#define WIFI_PASS "password1"

int status = WL_IDLE_STATUS;
WiFiClient client;
WebSocketsClient webSocket;
unsigned long lastPingTime = 0;
const unsigned long pingInterval = 5000; 

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Afbrudt! Forsøger at genoprette forbindelse...");
      break;
      
    case WStype_CONNECTED:
      Serial.println("[WSc] Forbundet!");
      break;
      
    case WStype_TEXT: {
      String command = String((char *)payload);
      Serial.print("[WSc] Modtaget kommando: ");
      Serial.println(command);
      
      if (command == "forward") {
        Serial.println("Udfører: Kør fremad");
        // TODO: Implementer motorstyring for fremad
      }
      else if (command == "backward") {
        Serial.println("Udfører: Bak");
        // TODO: Implementer motorstyring for bak
      }
      else if (command == "stop") {
        Serial.println("Udfører: Stop");
        // TODO: Implementer motorstyring for stop
      } else {
        Serial.println("Kommando ikke genkendt!");
      }
      break;
    }
    
    case WStype_PING:
      Serial.println("[WSc] Modtaget Ping");
      break;

    case WStype_PONG:
      Serial.println("[WSc] Modtaget Pong");
      break;
    
    case WStype_ERROR:
      Serial.println("[WSc] Fejl! Forsøger at genoprette forbindelse");
      webSocket.disconnect();
      delay(5000);
      setup();
      break;

    default:
      Serial.println("Kommando slet ikke genkendt!");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Giv tid til at starte op

  // Check WiFi modul
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Kommunikation med WiFi modul fejlede!");
    while (true);
  }

  // Forbind til WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Forbinder til SSID: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(5000);
  }

  Serial.println("Forbundet til WiFi");
  Serial.print("IP Adresse: ");
  Serial.println(WiFi.localIP());

  // Konfigurer WebSocket
  webSocket.beginSSL("hyperdrivelabs.onrender.com", 443, "/ws");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);
}

void loop() {
  webSocket.loop();
  
  // Send ping med jævne mellemrum
  unsigned long currentTime = millis();
  if (currentTime - lastPingTime > pingInterval) {
    webSocket.sendPing();
    lastPingTime = currentTime;
  }
}