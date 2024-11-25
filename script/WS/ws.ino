#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <WebSocketsClient.h>

#define WIFI_SSID "GalaxyS22123"
#define WIFI_PASS "password1"

int status = WL_IDLE_STATUS;
WiFiClient client;
WebSocketsClient webSocket;
unsigned long lastPingTime = 0;
const unsigned long pingInterval = 5000; 

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
      }
      else if (command == "backward") {
        Serial.println("[WSc] ✓ Matcher 'backward'");
        Serial.println("Udfører: Bak");
      }
      else if (command == "stop") {
        Serial.println("[WSc] ✓ Matcher 'stop'");
        Serial.println("Udfører: Stop");
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

void setup() {
  Serial.begin(115200);
  delay(1000);
  
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

  // Opdateret WebSocket konfiguration
  webSocket.beginSSL("hyperdrivelabs.onrender.com", 443, "/ws");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);
  
  // Tilføj ekstra debug information
  Serial.println("WebSocket konfiguration færdig");
  Serial.println("Venter på beskeder...");
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