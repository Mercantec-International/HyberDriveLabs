#ifndef HYPDERDRIVECAR_H
#define HYPDERDRIVECAR_H

#include <Arduino_MKRIoTCarrier.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Ultrasonic.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <bsec.h>

class HyperDriveCar {
  private:
    MKRIoTCarrier carrier;
    HttpClient* httpClient;
    WiFiSSLClient wifi;
    WiFiClient client;

    float temp;
    float humidity;
    float x, y, z;
    float lastHumidity, lastTemp;
    
    int lastX, lastY, lastZ, lastLength;
    int lastMillis;
    
    const char* fileName = "savedval.txt";
    String deviceId = "8d414a937e634a16945e5d17adc5e04a";
    
    const char* apiUrl = "";
    String ssidAP = "HyperDrive-Hotspot";

    int status = WL_IDLE_STATUS;


  public:
    void begin();
    void handleTempHumid();
    void handleDistance();
    void handleData();
    String sendData(String body, String action);
    void writeToSD(String input, bool clearSDFile);
    String readFromSD();
    void updateStatus();
    void setupWebpage();
    void beginNetwork();
    void connectToWiFi(String ssid, String password);
    bool isConnected();
};

#endif // HYPDERDRIVECAR_H
