#pragma once

#define DEFAULT_NAME "ehsensor8"
#include <ESP8266mDNS.h>
//#include <ESP8266LLMNR.h>

class InitDiscovery : public Runnable {
private:
  uint32_t run() {
    // MDNS
    if (!MDNS.begin(DEFAULT_NAME)) {
      Serial.println("Error setting up MDNS responder!");
    } else {
      MDNS.addService("http", "tcp", 80);  // Add service to MDNS-SD
      Serial.println("mDNS responder started");
    }
    // LLMNR
    LLMNR.begin(DEFAULT_NAME);
    Serial.println("LLMNR reponder started");
    return RUN_DELETE;
  }
};
