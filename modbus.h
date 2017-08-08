#pragma once

#include <Modbus.h>
#include <ModbusIP_ESP8266.h>

//Modbus Registers Offsets (0-9999)
const int LED_COIL = 100;
//Used Pins
const int ledPin = 0; //GPIO0

class ModBusSlave : public Runnable, public ModbusIP {
private:
  bool initDone = false;
  uint32_t run() {
    if (!initDone) {
      config();
      pinMode(ledPin, OUTPUT);
      addCoil(LED_COIL);
      Serial.println("Modbus server is running...");
    } else {
      task();
    }
    return 100;
  }
};
