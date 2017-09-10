#pragma once

//To upload through terminal you can use: curl -F "image=@firmware.tar" ehsensor8.local/update

extern Web* web;
String adminUsername = "admin";
String adminPassword = "password3";

void updateHandle() {
      if(!web->authenticate(adminUsername.c_str(), adminPassword.c_str())) {
        return web->requestAuthentication();
      }
      BUSY
      web->sendHeader("Connection", "close");
      web->sendHeader("Refresh", "10; url=/");
      web->send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
}

void updateUploadHandle() {
  BUSY
      HTTPUpload& upload = web->upload();
      if(upload.status == UPLOAD_FILE_START){
        //WiFiUDP::stopAll();
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

class WUpdate : public Runnable {
  public:
  WUpdate() {
    autodelete();
  }
  private:
  uint32_t run() {
    web->on("/update", HTTP_POST, updateHandle, updateUploadHandle);//Update firmware
    return RUN_DELETE;
  }
};
