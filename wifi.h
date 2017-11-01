#pragma once

#ifdef ESP8266
 #include <ESP8266WiFi.h>
 #include <ESP8266mDNS.h>
 #include <ESP8266LLMNR.h>
#else
 #include <WiFi.h>
 #include <ESPmDNS.h>
#endif

uint32_t wifiWait() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    event.wifiReady++;
    if (!MDNS.begin(DEFAULT_NAME)) {
      Serial.println("Error setting up MDNS responder!");
    } else {
      MDNS.addService("http", "tcp", 80);  // Add service to MDNS-SD
      Serial.println("mDNS responder started");
    }
    // LLMNR
   #ifdef ESP8266
    LLMNR.begin(DEFAULT_NAME);
    Serial.println("LLMNR reponder started");
   #endif
    return RUN_DELETE;
  }
  Serial.print(".");
  return 500;
}

uint32_t wifiInit() {
    WiFi.begin();
    taskAdd(wifiWait);
    return RUN_DELETE;
}
