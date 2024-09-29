//
// Created by painnick on 2024-09-28.
//

#ifndef BODY_MCU_BLUETOOTH_CONTROLLER_H
#define BODY_MCU_BLUETOOTH_CONTROLLER_H

#include "BluetoothSerial.h"
#include <Arduino.h>

#include "command.h"

#define BT_TAG "BLUETOOTH"

class BluetoothController {
 public:
  void begin(String name);
  Command receive();
  void println(const char c[]);

 protected:
  BluetoothSerial serial;
};

#endif// BODY_MCU_BLUETOOTH_CONTROLLER_H
