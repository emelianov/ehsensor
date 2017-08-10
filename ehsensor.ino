//
// ehsensor
// Home automation solution
//
// ESP8266 ModBus TCP Slave Device
//
struct events {
  uint16_t webReady;
  uint16_t ntpReady;
  uint16_t wifiReady;
};
events event = {0};

#include <Runnable.h>
#include "wifi.h"
#include <ESP8266WebServer.h>
ESP8266WebServer http(80);
#include "discovery.h"
#include "modbus.h"



class Web : public Runnable {
public:
  Web(uint16_t* s) {
    raiseSemaphore = s;
  }
private:
  bool initDone = false;
  uint16_t* raiseSemaphore;
  uint32_t run() {
    if (!initDone) {
      http.begin();
      runWithoutSemaphore();
      initDone = true;
      *raiseSemaphore = *raiseSemaphore + 1;
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
  web = new Web(&event.webReady);
  web->runWithSemaphore(&event.wifiReady);
  discovery = new InitDiscovery();
  discovery->runWithSemaphore(&event.wifiReady);
  mb = new ModBusSlave();
  mb->runWithSemaphore(&event.wifiReady);
   
}

void loop() {
  runnableLoop();
}
