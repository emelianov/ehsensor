#pragma once

// Uncomment for MDNS (macOS) and LLMNR (Windows) support otherwise SSPD (Windows) is used
#define MACOS
#define DEFAULT_NAME "ehsensor8"
#ifdef MACOS
#include <ESP8266mDNS.h>
#ifdef ESP8266LLMNR_H
#include <ESP8266LLMNR.h>
#endif
#else
#include <ESP8266SSDP.h>
#endif

class InitDiscovery : public Runnable {
private:
  uint32_t run() {
#ifdef MACOS
    // MDNS
    if (!MDNS.begin(DEFAULT_NAME)) {
      Serial.println("Error setting up MDNS responder!");
    } else {
      MDNS.addService("http", "tcp", 80);  // Add service to MDNS-SD
      Serial.println("mDNS responder started");
    }
    #ifdef ESP8266LLMNR_H
    // LLMNR
    LLMNR.begin(DEFAULT_NAME);
    Serial.println("LLMNR reponder started");
    #endif
#else
    // SSPD
    web.on("/description.xml", HTTP_GET, [](){
      SSDP.schema(http.client());
    });
    Serial.printf("Starting SSDP...\n");
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName("EHSensor ModBus TCP Slave");
    SSDP.setSerialNumber(ESP.getChipId());
    SSDP.setURL("index.html");
    SSDP.setModelName("EHSensor");
    SSDP.setModelNumber("0.1");
    SSDP.setModelURL("https://github.com/emelianov/ehsensor");
    SSDP.setManufacturer("Alexander Emelianov");
    SSDP.setManufacturerURL("http://github.com/emelianov");
    SSDP.setDeviceType("urn:schemas-upnp-org:device:SensorManagement:1");
    SSDP.begin();
    Serial.printf("SSPD responder started\n");
#endif
    return RUN_DELETE;
  }
};
