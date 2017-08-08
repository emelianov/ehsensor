//
// ehsensor
// Home automation solution
//
// ESP8266 ModBus TCP Slave Device
//

#include <Runnable.h>
#include "wifi.h"
#include <ESP8266WebServer.h>
ESP8266WebServer http(80);
#include "discovery.h"
#include "modbus.h"

struct events {
  uint16_t webReady;
  uint16_t ntpReady;
  uint16_t wifiReady;
};
events event = {0};

class Web : public Runnable {
private:
  bool initDone = false;
  uint32_t run() {
    if (!initDone) {
      http.begin();
      runWithoutSemaphore();
      Serial.println("Web server is running...");
    } else {
      http.handleClient();
    }
    return 100;
  }
};

InitWiFi* wifi;
Web* web;
InitDiscovery* discovery;
ModBusSlave* mb;

void setup(void)
{  
  Serial.begin(74880);
  
  // Connect to WiFi network
  wifi = new InitWiFi(&event.wifiReady);
  wifi->runNow();
  web = new Web();
  web->runWithSemaphore(&event.wifiReady);
  discovery = new InitDiscovery();
  discovery->runWithSemaphore(&event.webReady);
  mb = new ModBusSlave();
  mb->runWithSemaphore(&event.wifiReady);
}

void loop() {
  runnableLoop();
}
