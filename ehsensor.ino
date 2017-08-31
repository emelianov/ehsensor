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

template <typename T>
class SReg {
public:
  T val;
  uint16_t event = 0;
  void set(T v) {
    val = v;
    age = 0;
    event++;
  }
  T get() {
    return val;
  }
  String toString() {
    return String(val);
  }
  uint16_t getAge() {
    return age;
  }
  bool isValid() {
    return true;
  }
private:
  uint16_t age = 0xFFFF;
};

#define REG_COUNT 32
SReg<int> iReg[REG_COUNT];
SReg<float> fReg[REG_COUNT];

events event = {0};

#include <Runnable.h>
#include "wifi.h"
#include "web.h"
#include "discovery.h"
#include "modbus.h"
#include "ds1820.h"

InitWiFi* wifi;
Web* web;
InitDiscovery* discovery;
ModBusSlave* mb;
TSensors* ds;

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
  ds = new TSensors();                    // Start 1-Wire temperature sensors
  ds->runNow();
}

void loop() {
  runnableLoop();
}
