#pragma once

#include <ESP8266WiFi.h>

class InitWiFi : public Runnable {
public:
  InitWiFi(uint16_t* s) {
    WiFi.begin();
    raiseSemaphore = s;
  }
private:
  uint16_t* raiseSemaphore;
  uint32_t run() {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
//      Serial.print("Connected to ");
//      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
     *raiseSemaphore = *raiseSemaphore + 1;
     //event.wifiReady++;
     return RUN_DELETE;
    }
    Serial.print(".");
    return 500;
  }
};
