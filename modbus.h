#pragma once

#include <Modbus.h>
#include <ModbusIP_ESP8266.h>

//Modbus Registers Offsets (0-9999)
const int LED_COIL = 100;
//Used Pins
const int ledPin = D0; //GPIO0
//Modbus Registers Offsets (0-9999)
const int SWITCH_ISTS = 101;
//Used Pins
const int switchPin = D3; //GPIO0

class ModBusSlave : public Runnable, public ModbusIP {
private:
  bool initDone = false;
  uint32_t run() {
    if (!initDone) {
      config();
      pinMode(ledPin, OUTPUT);
      pinMode(switchPin, INPUT);
      addCoil(LED_COIL);
      addIsts(SWITCH_ISTS);
      initDone = true;
      Serial.println("Modbus server is running...");
    } else {
      task();
      Ists(SWITCH_ISTS, digitalRead(switchPin));
      digitalWrite(ledPin, Coil(LED_COIL));
    }
    return 100;
  }
};
