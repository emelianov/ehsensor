#pragma once

#include <DallasTemperature.h>

#define DS_INTERVAL 10000
#define DS_MAX_COUNT 16
#define CFG_SENSORS "/sensors.xml"

struct sensor {
  DeviceAddress device;
  int16_t map;
  String toString() {
    char szRet[24];
    sprintf_P(szRet,PSTR("%X%X%X%X%X%X%X%X"), device[0], device[1], device[2], device[3], device[4], device[5], device[6], device[7]);
    return String(szRet);
  }
};

OneWire oneWire(OW_PIN);

class TSensors : public DallasTemperature {
  public:
  TSensors() : DallasTemperature(&oneWire) {
    begin();
    setResolution(12);
    setWaitForConversion(false);
  }
};  

TSensors* ds;
sensor sens[DS_MAX_COUNT];
  uint32_t dsResponse();
  uint32_t dsRequest() {
    ds->requestTemperatures();
    taskAddWithDelay(dsResponse, 250);
    return RUN_DELETE;
  }
  
  uint32_t dsResponse() {
    uint8_t i;
    DeviceAddress zerro;
    memset(zerro, 0, sizeof(DeviceAddress));
    for (i = 0; i < DS_MAX_COUNT; i++) {
      if (memcmp(sens[i].device, zerro, sizeof(DeviceAddress)) != 0) {
        float t = ds->getTempC(sens[i].device);
        if (t !=  DEVICE_DISCONNECTED_C) {
          if (sens[i].map != -1) {
            fReg[sens[i].map].set(t);
          }
        }
      }
    }
    return DS_INTERVAL;
  }

  bool dsFindNew() {
    bool newDeviceAdded = false;
    for (uint8_t i = 0; i < ds->getDeviceCount(); i++) {
      DeviceAddress deviceFound;
      ds->getAddress(deviceFound, i);
      uint8_t j = 0;
      for (j; j < DS_MAX_COUNT; j++) {
        if (memcmp(sens[j].device, deviceFound, sizeof(DeviceAddress)) == 0) break;
      }
      if (j >= DS_MAX_COUNT) {
        DeviceAddress zerro;
        memset(zerro, 0, sizeof(DeviceAddress));
        for (j = 0; j < DS_MAX_COUNT; j++) {
          if (memcmp(sens[j].device, zerro, sizeof(DeviceAddress)) == 0) {
            memcpy(sens[j].device, deviceFound, sizeof(DeviceAddress));
            newDeviceAdded = true;
            Serial.println(sens[j].toString());
            sens[j].map = 0;
            break;
          }
        }
      }
    }
    return newDeviceAdded;
  }

uint32_t dsInit() {
  ds = new TSensors();
  for (uint8_t i = 0; i < DS_MAX_COUNT; i++) {
    sens[i].map = -1;
    memset(sens[i].device, 0, sizeof(DeviceAddress));    // Fill device id with 0
  }
  dsFindNew();
  taskAdd(dsRequest);
  return RUN_DELETE;
}

/*
extern TinyXML xml;
extern String xmlOpen;
extern String xmlTag;
extern String xmlData;
extern String xmlAttrib;
extern void XML_callback(uint8_t statusflags, char* tagName, uint16_t tagNameLen, char* data, uint16_t dataLen);

bool saveSensors() {
   File configFile = SPIFFS.open(F(CFG_SENSORS), "w");
   if (configFile) {
    char buf[200];
    sprintf_P(buf, PSTR("<?xml version = \"1.0\" ?>\n<sensors>\n"));
    configFile.write((uint8_t*)buf, strlen(buf));
    for (uint8_t i = 0; i < DEVICE_MAX_COUNT; i++) {
      sprintf_P(buf, PSTR("<sensor><id>%02X%02X%02X%02X%02X%02X%02X%02X</id><gid>%d</gid><name>%s</name></sensor>\n"),
                 sens[i].device[0], sens[i].device[1], sens[i].device[2], sens[i].device[3], sens[i].device[4], sens[i].device[5], sens[i].device[6], sens[i].device[7],
                 sens[i].gid, sens[i].name.c_str());
      configFile.write((uint8_t*)buf, strlen(buf));
    }
    sprintf_P(buf, PSTR("</sensors>"));
    configFile.write((uint8_t*)buf, strlen(buf));
    configFile.close();
    return true;
   }
   return false;  
}

bool readSensors() {
  int16_t i;
  for (i = 0; i < DEVICE_MAX_COUNT; i++) {
    sens[i].tCurrent = DEVICE_DISCONNECTED_C;
    sens[i].name = String(i);
    sens[i].gid = 0;
    sens[i].age = DEVICE_AGE_DEF;
    memset(sens[i].device, 0, sizeof(DeviceAddress));    //Fill device id with 0
  }
  i = 0;
  bool sensorOpen = false;
  File configFile = SPIFFS.open(CFG_SENSORS, "r");
  if (configFile) {
   xml.reset();
   xmlTag = "";
   xmlOpen = "";
   char c;
   while (configFile.read((uint8_t*)&c, 1) == 1) {
    xml.processChar(c);
    if (xmlTag != "" || xmlOpen != "") {
       if 
      (xmlOpen.endsWith(F("/sensor"))) {
        if (sensorOpen) {
          i++;
        } else {
          sensorOpen = true;
        }
        //xmlOpen = "";
        if (i >= DEVICE_MAX_COUNT) {
          break;
        }
       } else if
      (xmlTag.endsWith(F("/id"))) {
        for (uint8_t j = 0; j < 8; j++) {
          sens[i].device[j] = strtol(xmlData.substring(j*2, j*2+2).c_str(), NULL, 16);
        }
       } else if
      (xmlTag.endsWith(F("/gid"))) {
        sens[i].gid = xmlData.toInt();
       } else if
      (xmlTag.endsWith(F("/name"))) {
        sens[i].name = xmlData;
       }
    }
    xmlTag = "";
    xmlOpen = "";
   }
   configFile.close();
   return true;
  }
  return false;
}

*/
