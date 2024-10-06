//
// Created by painnick on 2024-09-28.
//

#ifndef BODY_MCU_BLUETOOTH_CONTROLLER_H
#define BODY_MCU_BLUETOOTH_CONTROLLER_H

#include "BluetoothSerial.h"
#include <Arduino.h>

class BluetoothController;

enum BluetoothCommandType {
  BT_UNKNOWN,
  BT_HELP,
  BT_ACK,
  BT_WAKE_UP,
  BT_SLEEP,
  BT_STOP,
  BT_TURN_LEFT,
  BT_TURN_RIGHT,
  BT_LIGHT_ON,
  BT_LIGHT_OFF,
  BT_WIFI_ON,
  BT_WIFI_OFF,
  BT_PLAY_MUSIC,
  BT_FOOL,
  BT_LOOK_AT_ME,
  BT_ATTENTION,
  BT_DFPLAYER_VOLUME,
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
  BluetoothController(String btName) : name(btName) {}

  void init(BluetoothControllerCallback callback);

  void begin();

  void loop();

  void println(const char c[]);

  void printHelp();

  void close();

  bool isBluetoothOn = false;

 protected:
  String name;
  BluetoothSerial serial;
  BluetoothControllerCallback proc;
};

#endif// BODY_MCU_BLUETOOTH_CONTROLLER_H
