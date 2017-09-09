#pragma once

void handleUpdate() {
      if(!server.authenticate(adminUsername.c_str(), adminPassword.c_str())) {
        return server.requestAuthentication();
      }
      BUSY
      server.sendHeader("Connection", "close");
      server.sendHeader("Refresh", "15; url=/");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
}

void handleUpdateUpload() {
  BUSY
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        //WiFiUDP::stopAll();
        for (uint8_t i = 0; i < RELAY_COUNT; i++) {
          if (relays[i].pin != -1) { _off(i);}
        }
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
  IDLE
}

