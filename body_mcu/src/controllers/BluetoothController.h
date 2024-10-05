//
// Created by painnick on 2024-09-28.
//

#ifndef BODY_MCU_BLUETOOTH_CONTROLLER_H
#define BODY_MCU_BLUETOOTH_CONTROLLER_H

#include "BluetoothSerial.h"
#include <Arduino.h>

#include "command.h"

class BluetoothController;

enum BluetoothCommandType {
  BT_UNKNOWN,
  BT_HELP,
  BT_ACK,
  BT_WIFI_ON,
  BT_WIFI_OFF,
  BT_WARN,
  BT_RANDOM_LIGHT1,
  BT_RANDOM_LIGHT2,
  BT_LIGHT_ON,
  BT_LIGHT_OFF,
  BT_TURN_LEFT,
  BT_TURN_RIGHT,
};

typedef struct {
  BluetoothCommandType commandType;
  String msg;
  String desc;
} BluetoothCommandInfo;

String ToString(const BluetoothCommandInfo &cmd);

BluetoothCommandType ToBluetoothCommandType(String &msg);

typedef std::function<void(BluetoothController *controller,
                           BluetoothCommandType commandType,
                           String &msg)> BluetoothControllerCallback;

class BluetoothController {
 public:
  void init(BluetoothControllerCallback callback);

  void begin(String name);

  void loop();

  void println(const char c[]);

  void printHelp();

 protected:
  BluetoothSerial serial;
  BluetoothControllerCallback proc;
};

#endif// BODY_MCU_BLUETOOTH_CONTROLLER_H
