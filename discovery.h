#pragma once

#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>

class InitDiscovery : public Runnable {
private:
  uint32_t run() {
    if (!MDNS.begin("ehsensor")) {
      Serial.println("Error setting up MDNS responder!");
    }
    // MDNS
    MDNS.addService("http", "tcp", 80);  // Add service to MDNS-SD
    Serial.println("mDNS responder started");
    // SSPD
    http.on("/description.xml", HTTP_GET, [](){
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

    return RUN_DELETE;
  }
};
