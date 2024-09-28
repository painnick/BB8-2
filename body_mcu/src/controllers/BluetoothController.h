//
// Created by painnick on 2024-09-28.
//

#ifndef BODY_MCU_BLUETOOTHCONTROLLER_H
#define BODY_MCU_BLUETOOTHCONTROLLER_H

#include <Arduino.h>
#include "BluetoothSerial.h"

#include "command.h"

class BluetoothController {
public:
  void begin(String name);
  Command receive();

  BluetoothSerial serial;
};

#endif // BODY_MCU_BLUETOOTHCONTROLLER_H
