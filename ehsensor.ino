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

#define PIN_ACT D4
#define BUSY    digitalWrite(PIN_ACT, LOW);
#define IDLE    digitalWrite(PIN_ACT, HIGH);


template <typename T>
class SReg {
public:
  T val;
  uint16_t event = 0;
  void set(T v) {
    val = v;
    event++;
  }
  T get() {
    return val;
  }
  String toString() {
    return String(val);
  }
};

#define REG_COUNT 32
SReg<int> iReg[REG_COUNT];
SReg<float> fReg[REG_COUNT];

events event = {0};

#include <Runnable.h>
#include "wifi.h"
InitWiFi* wifi;

#include "web.h"
//Web* web;

#include "discovery.h"
InitDiscovery* discovery;

#include "modbus.h"
ModBusSlave* mb;

void setup(void)
{  
  Serial.begin(74880);  
  wifi = new InitWiFi(&event.wifiReady);  // Connect to WiFi network
  wifi->runNow();
  web = new Web(80, &event.webReady);     // Start Web-server
  web->runWithSemaphore(&event.wifiReady);
  discovery = new InitDiscovery();        // Start discovery services
  discovery->runWithSemaphore(&event.wifiReady);
  mb = new ModBusSlave();                 // Strat ModBus Slave
  mb->runWithSemaphore(&event.wifiReady); 
}

void loop() {
  runnableLoop();
}
