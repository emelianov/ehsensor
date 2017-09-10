#pragma once
#include <ESP8266WebServer.h>
#include <FS.h>

void handleGenericFile();

class Web : public ESP8266WebServer, public Runnable {
public:
  Web(uint16_t port, uint16_t* s) : ESP8266WebServer(port) {
    raiseSemaphore = s;
  }
  String getContentType(String filename){
    if(hasArg(F("download"))) return "application/octet-stream";
    else if(filename.endsWith(".htm")) return "text/html";
    else if(filename.endsWith(".html")) return "text/html";
    else if(filename.endsWith(".css")) return "text/css";
    else if(filename.endsWith(".js")) return "application/javascript";
    else if(filename.endsWith(".png")) return "image/png";
    else if(filename.endsWith(".gif")) return "image/gif";
    else if(filename.endsWith(".jpg")) return "image/jpeg";
    else if(filename.endsWith(".ico")) return "image/x-icon";
    else if(filename.endsWith(".xml")) return "text/xml";
    else if(filename.endsWith(".pdf")) return "application/x-pdf";
    else if(filename.endsWith(".zip")) return "application/x-zip";
    else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
  }
  bool handleFileRead(String path){
    if(path.endsWith(F("/"))) path += F("index.html");
    String contentType = getContentType(path);
    String pathWithGz = path + F(".gz");
    if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
      if(SPIFFS.exists(pathWithGz))
        path += F(".gz");
      sendHeader("Connection", "close");
      sendHeader("Cache-Control", "no-store, must-revalidate");
      sendHeader("Access-Control-Allow-Origin", "*");
      File file = SPIFFS.open(path, "r");
      size_t sent = streamFile(file, contentType);
      file.close();
      return true;
    }
    return false;
  }
private:
  bool initDone = false;
  uint16_t* raiseSemaphore;
  uint32_t run() {
    if (!initDone) {
      begin();
      runWithoutSemaphore();
      onNotFound(handleGenericFile);
      initDone = true;
      *raiseSemaphore = *raiseSemaphore + 1;
      Serial.println("Web server is running...");
    } else {
      handleClient();
    }
    return 100;
  }
};

Web* web;

void handleGenericFile() {
  BUSY
  if(!web->handleFileRead(web->uri()))
    web->send(404, "text/plain", "FileNotFound");
  IDLE
}
