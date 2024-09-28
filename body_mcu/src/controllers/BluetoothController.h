//
// Created by painnick on 2024-09-28.
//

#ifndef BODY_MCU_BLUETOOTHCONTROLLER_H
#define BODY_MCU_BLUETOOTHCONTROLLER_H

#include <Arduino.h>
#include "BluetoothSerial.h"

enum BTCommand : uint64_t {
  BT_UNKNOWN = 0x43434343,
  BT_WAKE_UP = 0x00000001,
  BT_TURN_LEFT = 0x01000100,
  BT_TURN_RIGHT = 0x01000200,
  BT_PLAY_MUSIC = 0x70000000,
  BT_FOOL = 0x44444444,
  BT_STOP = 0x00000000,
  BT_TURN_ON = 0x02000001,
  BT_TURN_OFF = 0x02000002,
  BT_WHERE_ARE_YOU = 0xFFFFFF01,
};

extern String ToString(BTCommand cmd);

class BluetoothController {
public:
  void begin(String name);
  BTCommand receive();

protected:
  BluetoothSerial serial;
};

#endif // BODY_MCU_BLUETOOTHCONTROLLER_H
