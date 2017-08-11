#pragma once
#include <ESP8266WebServer.h>

class Web : public ESP8266WebServer, public Runnable {
public:
  Web(uint16_t port, uint16_t* s) : ESP8266WebServer(port) {
    raiseSemaphore = s;
  }
private:
  bool initDone = false;
  uint16_t* raiseSemaphore;
  uint32_t run() {
    if (!initDone) {
      begin();
      runWithoutSemaphore();
      initDone = true;
      *raiseSemaphore = *raiseSemaphore + 1;
      Serial.println("Web server is running...");
    } else {
      handleClient();
    }
    return 100;
  }
};
