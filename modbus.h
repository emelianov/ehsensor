#pragma once

#include <ModbusIP_ESP8266.h>

//Modbus Registers Offsets (0-9999)
const int LED_COIL = 100;
//Used Pins
const int ledPin = PIN_LED; //GPIO0
//Modbus Registers Offsets (0-9999)
const int SWITCH_ISTS = 101;
//Used Pins
const int switchPin = PIN_KEY; //GPIO0

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

class ModBusSlave : public ModbusIP {
public:
  ModBusSlave() {
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
  }
};

ModBusSlave* mb;

uint32_t modbusLoop() {
      mb->task();
      mb->Ists(SWITCH_ISTS, digitalRead(switchPin));
      digitalWrite(ledPin, mb->Coil(LED_COIL));
      return 100;
}

uint32_t modbusInit() {
  mb = new ModBusSlave();
  taskAdd(modbusLoop);
  Serial.println("Modbus server is running..."); 
  return RUN_DELETE;
}
