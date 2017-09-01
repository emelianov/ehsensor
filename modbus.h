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

union splitFloat {
  float f;
  uint16_t w[2];
};

uint16_t cbIreg(TRegister* reg, uint16_t val) {
  if (reg->address >= IREG_BASE) {
    uint16_t r = reg->address - IREG_BASE;
    if (r < REG_COUNT) {
      splitFloat t;
      t.f = fReg[0].get();
      if (r == 0) {
        return t.w[0];
      } else if (r == 1) {
        return t.w[1];
      }
    }
  }
  return val;
}

bool cbConn(IPAddress ip) {
  Serial.println(ip);
  return true;
}

class ModBusSlave : public Runnable, public ModbusIP {
private:
  bool initDone = false;
  uint32_t run() {
    if (!initDone) {
      onConnect(cbConn);
      begin();
      pinMode(ledPin, OUTPUT);
      pinMode(switchPin, INPUT);
      addCoil(LED_COIL);
      addIsts(SWITCH_ISTS);
      addIreg(0);
      addIreg(1);
      onGet(IREG(0), cbIreg);
      onGet(IREG(1), cbIreg);
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
