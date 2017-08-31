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
const int switchPin = 0; //GPIO0

uint16_t cbIreg(TRegister* reg, uint16_t val) {
  if (reg->address >= IREG_BASE) {
    uint16_t r = reg->address - IREG_BASE;
    if (r < REG_COUNT) {
      uint32_t t = (uint32_t)fReg[r].get();
      if (r == 0) {
        
      } else if (r == 1) {
        
      }
    }
  }
  return val;
}

class ModBusSlave : public Runnable, public ModbusIP {
private:
  bool initDone = false;
  uint32_t run() {
    if (!initDone) {
      begin();
      pinMode(ledPin, OUTPUT);
      pinMode(switchPin, INPUT);
      addCoil(LED_COIL);
      addIsts(SWITCH_ISTS);
      addIreg(IREG(0));
      addIreg(IREG(1));
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
