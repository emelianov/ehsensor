#pragma once

#ifdef ESP8266
 #include <ESP8266WiFi.h>
 #include <ESP8266mDNS.h>
 #include <ESP8266LLMNR.h>
 #include <time.h>
#else
 #include <WiFi.h>
 #include <ESPmDNS.h>
#endif

#define NTP_CHECK_DELAY 300000

uint32_t initNtp() {
  if (time(NULL) == 0) {
    configTime(tz * 3600, 0, ntp[0].c_str(), ntp[1].c_str(), ntp[2].c_str());
    return NTP_CHECK_DELAY;
  }
  event.ntpReady++;
  return RUN_DELETE;
}

uint32_t wifiWait() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    event.wifiReady++;
    
    if (!MDNS.begin(DEFAULT_NAME)) {
      Serial.print("[mDNS: failed]");
    } else {
      MDNS.addService("http", "tcp", 80);  // Add service to MDNS-SD
      Serial.print("[mDNS: started]");
    }
    // LLMNR
   #ifdef ESP8266
    LLMNR.begin(DEFAULT_NAME);
    Serial.println("[LLMNR: started]");
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
