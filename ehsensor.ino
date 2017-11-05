//
// ehsensor
// Home automation solution
//
// ESP8266 ModBus TCP Slave Device
//
#define VERSION "0.1.2"
#ifdef ESP8266
 #define PIN_ACT D4
 #define BUSY    digitalWrite(PIN_ACT, LOW);
 #define IDLE    digitalWrite(PIN_ACT, HIGH);
 #define OW_PIN D2
 #define PIN_LED D0
 #define PIN_KEY 0
#else
 #define PIN_ACT TX
 #define BUSY    digitalWrite(PIN_ACT, LOW);
 #define IDLE    digitalWrite(PIN_ACT, HIGH);
 #define OW_PIN 2
 #define PIN_LED TX
 #define PIN_KEY 0
#endif
#define DEFAULT_NAME "ehsensor8"
#define NTP1 "192.168.30.30"
#define NTP2 ""
#define NTP3 ""
#define TZ 5
#define DEFAULT_ADMIN_NAME "admin"
#define DEFAULT_PASSWORD "password3"

String adminUsername = DEFAULT_ADMIN_NAME;
String adminPassword = DEFAULT_PASSWORD;
String name = DEFAULT_NAME;
String ntp[3] {NTP1, NTP2, NTP3};
int8_t tz = TZ;
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
#ifdef ESP8266
 #include <FS.h>
#else
 #include <SPIFFS.h>
#endif
#include <Run.h>
#include "wifi.h"
#include "web.h"
#include "update.h"
#include "ds1820.h"
#include "modbus.h"

uint32_t restartESP() {
  ESP.restart();
  return RUN_DELETE;
}

uint32_t formatSPIFFS() {
  SPIFFS.end();
  SPIFFS.format();
  SPIFFS.begin();
  return RUN_DELETE;
}

uint32_t global() {
  SPIFFS.begin();
  return RUN_DELETE;
}
void setup(void)
{
 #ifdef ESP8266
  Serial.begin(74880);
 #else
  Serial.begin(115200);
 #endif
  taskAdd(wifiInit);    // Connect to WiFi network & Start discovery services
  taskAdd(global);
  taskAdd(modbusInit);  // Strat ModBus Slave
  taskAdd(dsInit);      // Start 1-Wire temperature sensors
  taskAdd(webInit);     // Start Web-server
  taskAdd(updateInit);  // Add update service
}

void loop() {
  taskExec();
  yield();
}
