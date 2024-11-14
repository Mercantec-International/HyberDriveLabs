#include "HyperDriveCar.h"
WiFiServer server(80);

void HyperDriveCar::begin() {
  Serial.begin(9600); 

  carrier.noCase();
  carrier.begin();
    
  beginNetwork();
}

// Hande temp and humidity sensor
void HyperDriveCar::handleTempHumid() {
  float humidity = carrier.Env.readHumidity();
  float temp = carrier.Env.readTemperature();

  // Print value if it has changed
  if (humidity != lastHumidity || temp != lastTemp) {
    lastHumidity = humidity;
    Serial.print("Humidity: ");
    Serial.println(humidity);
    lastTemp = temp;
    Serial.print("Temp: ");
    Serial.println(temp);


    int intTemp = temp*100;
    int intHumid = humidity*100;
    
    // TODO: FIX BODY AND SEND
    String postBody = "";
  }
}

// Handle distance sensor
void HyperDriveCar::handleDistance() {
  Ultrasonic ultrasonic(A6);
  long distanceInCm = ultrasonic.MeasureInCentimeters();
  
  // If the change in distance is big enough and it has been more than 10 seconds since last request
  if (distanceInCm < 5) {
    // Create request body and add to batch
    // TODO: FIX BODY AND SEND
    String body = "";
    Serial.println(String(distanceInCm));

    lastLength = distanceInCm;
  }
}

// Handles reading all data, connection to wifi and sending data to wifi
void HyperDriveCar::handleData() {
  if (!isConnected()) { 
    // Check if a wifi has been saved
    if (SD.exists(fileName)) {
      // Get saved data
      String sdContent = readFromSD();

      // Get SSID from saved data
      String savedSSID = sdContent.substring(sdContent.indexOf("ssid") + 7);
      savedSSID = savedSSID.substring(0, savedSSID.indexOf("\""));
      
      // Get password from saved data
      String savedPass = sdContent.substring(sdContent.lastIndexOf("=")+2);
      savedPass = savedPass.substring(0, savedPass.indexOf("\""));

      // Attempt to connect to wifi
      connectToWiFi(savedSSID, savedPass);
    } else {
      // If no connection use accesspoint to get wifi
      updateStatus();
      setupWebpage();
    }
  } else {
    // If connection handle all sensors
    //handleTempHumid();
    handleDistance();
    handleWebSocket();
    
    delay(100);
  }
}

// Writes to sd card, can also clear and rewrite entire file
void HyperDriveCar::writeToSD(String input, bool clearSDFile) {  
  if (clearSDFile) {
    if (SD.exists(fileName)) {
      if (SD.remove(fileName)) {
        Serial.println("File successfully deleted");
      } else {
        Serial.println("Error when deleting file");
      }
    }
  }
  
  File SDFile = SD.open(fileName, FILE_WRITE);
  if (SD.exists(fileName)) {
    SDFile.println(input);
    SDFile.close();
  } else {
    Serial.print("SD status: ");
    if (!SD.begin(SD_CS)) {
      Serial.println("Not avaliable");
    } else {
      Serial.println("Avaliable");
    }
  }
}

// Reads data from sd card and returns
String HyperDriveCar::readFromSD() {
  String content = "";
  
  File SDFile = SD.open(fileName);
  if (SD.exists(fileName)) {
    while (SDFile.available()) {
      content += (char)SDFile.read();
    }
    SDFile.close();
  } else {
    if (SD.exists(fileName)) {
      Serial.println("Filen findes på SD-kortet");
    } else {
      Serial.println("Filen findes ikke på SD-kortet");
    }
  }

  return content;
}

// NETWORKING

// Initializes network and variables
void HyperDriveCar::beginNetwork() {
  if (WiFi.status() == WL_NO_MODULE) {
    while (true);
  }

  // 10.10.10.1
  WiFi.config(IPAddress(10, 10, 10, 1));

  status = WiFi.beginAP(ssidAP.c_str());

  if (status != WL_AP_LISTENING) {
    while (true);
  }

  server.begin();
}

// Updates status, used for making hotspot webpage
void HyperDriveCar::updateStatus() {
  if (status != WiFi.status()) {
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      Serial.println("Device connected to AP");
    } else {
      Serial.println("Device disconnected from AP");
    }
  }

  client = server.available();
}

// Makes the page where user can give ssid and password to local network
void HyperDriveCar::setupWebpage() {
  if (client) {
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (c == '\n') {
          if (currentLine.length() == 0) {
            Serial.println("Opened hotspot");
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<html><body>");
            client.print("<form method='get' action='/connect'>");
            client.print("<label for='ssid'>SSID:</label>");
            client.print("<input type='text' id='ssid' name='ssid'><br><br>");
            client.print("<label for='pass'>Password:</label>");
            client.print("<input type='text' id='pass' name='pass'><br><br>");
            client.print("<input type='submit' value='Connect'>");
            client.print("</form></body></html>");
            client.println();
            break;

          } else if (currentLine.startsWith("GET /connect")) {
              // Example: GET /connect?ssid=asdasg&21wag&pass=Merc1234%21 HTTP/1.1
              
              String givenSSID = currentLine.substring(currentLine.indexOf("=")+1, currentLine.indexOf("&pass"));
              String givenPass = currentLine.substring(currentLine.lastIndexOf("=")+1, currentLine.lastIndexOf(" HTTP"));
              
              // TODO: MAKE BETTER HEX TO ASCII TRANSLATOR
              givenSSID.replace("%21", "!");
              givenPass.replace("%21", "!");
              
              connectToWiFi(givenSSID, givenPass);
              
              currentLine = "";
          }
          currentLine = "";
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    // close the connection:
    client.stop();

  }
}

// Attempt to connect to wifi with given ssid and password, saves to sd card if connection is made
void HyperDriveCar::connectToWiFi(String ssid, String password) {

  Serial.println("Attempting connect with: " + ssid + " and: " + password);
  while (!isConnected()) {
    // Connect to WPA/WPA2 network:
    WiFi.begin(ssid.c_str(), password.c_str());
  }

  if (isConnected()) {
    Serial.println("Connected");
    String id = "";

    if (SD.exists(fileName)) {
      String sdContent = readFromSD();

      // Only get id
      id = sdContent.substring(sdContent.indexOf("\"id\"=") + 7);
      id = id.substring(0, id.indexOf("\"")-1);
    }
    
    if (!SD.exists(fileName) || id.length() < 25) {
      // Create new device in database and get ID
      //TODO: CREATE NEW DEVICE AND GET ID
      //String response = sendData("{}", "/api/SitSmartDevices");

      // EXAMPLE RESPONSE; NOT TO BE USED IN PRODUCTION
      String response = "{\"id\": \"ba4dde5fd6f444868b8fe98f7bb06df3\", \"tempHumiditys\": null, \"movements\": null, \"distanceObjects\": null, \"users\": null }";

      // GET ID FROM RESPONSE
      response = response.substring(response.indexOf("\"id\":")+7);
      id = response.substring(0, response.indexOf("\""));
      
      Serial.println(response);
      Serial.println(id);
    }


    String contentToSD = "{ \"id\"=\"" + id + "\",\"ssid\"=\"" + ssid + "\",\"password\"=\"" + password + "\" }";
    Serial.println("Writing to SD: " + contentToSD);

    writeToSD(contentToSD, true);
  } else {
    Serial.println("failed connection");
  }
}

// Sends given request and returns response, definetly works ;)
String HyperDriveCar::sendData(String body, String action) {
  Serial.println(body);
  /*httpClient->beginRequest();
  httpClient->post(action);
  httpClient->sendHeader("Content-Type", "application/json");
  httpClient->sendHeader("Content-Length", body.length());
  httpClient->sendHeader("accept", "text/plain");
  httpClient->beginBody();
  httpClient->print(body);
  httpClient->endRequest();
  
  // Gets response and prints in console
  int statusCode = httpClient->responseStatusCode();
  String response = httpClient->responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  return response;*/

}

// Check if wifi is connected
bool HyperDriveCar::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void HyperDriveCar::handleWebSocket() {
  if (isConnected()) {
    if (!webSocket.connected()) {
      Serial.println("Forbinder til WebSocket...");
      if (webSocket.connect(wsHost, wsPath, wsPort)) {
        Serial.println("Forbundet til WebSocket server");
      } else {
        Serial.println("WebSocket forbindelse fejlede");
        return;
      }
    }

    String data;
    if (webSocket.getData(data)) {
      Serial.println("Modtaget fra WebSocket: " + data);
      
      // Håndter de forskellige kommandoer
      if (data == "forward") {
        Serial.println("Kører fremad");
        // Implementer kode til at køre fremad
      }
      else if (data == "backward") {
        Serial.println("Bakker");
        // Implementer kode til at bakke
      }
      else if (data == "stop") {
        Serial.println("Stopper");
        // Implementer kode til at stoppe
      }
    }
  }
}
