//
// Created by painnick on 2024-09-28.
//

#ifndef BODY_MCU_COMMAND_ROUTER_H
#define BODY_MCU_COMMAND_ROUTER_H

#include "SoftwareSerial.h"

#define HEAD_TAG "HEAD"

class HeadCommander;

enum HeadCommandType {
  HEAD_UNKNOWN,
  HEAD_ACK,
  HEAD_LOG,
  HEAD_WIFI_ON,
  HEAD_WIFI_OFF,
  HEAD_FOOL,
  HEAD_LIGHT_ON,
  HEAD_LIGHT_OFF,
  HEAD_TURN_LEFT,
  HEAD_TURN_RIGHT,
  HEAD_TURN_LEFT2,
  HEAD_TURN_RIGHT2,
  HEAD_STOP_NOW,
  HEAD_WIFI_IS_ON,
  HEAD_WIFI_IS_OFF,
  HEAD_BT_ON,
  HEAD_BT_OFF,
};

typedef struct {
  HeadCommandType commandType;
  String msg;
  String desc;
} HeadCommandInfo;

String ToString(const HeadCommandInfo &cmd);

HeadCommandType ToHeadCommandType(String &msg);

typedef std::function<void(const HeadCommander *, HeadCommandType cmdType)> HeadCommanderCallback;

class HeadCommander {
 public:
  String cmdBuffer = "";

  explicit HeadCommander(SoftwareSerial &serial);

  ~HeadCommander();

  void init(HeadCommanderCallback callback);

  void begin(uint32_t baud, EspSoftwareSerial::Config config, int8_t rxPin, int8_t txPin);

  void loop();

  void send(HeadCommandType cmd);

  bool isWifiOn = false;

 protected:
  SoftwareSerial &serial;
  HeadCommanderCallback proc;
};

#endif// BODY_MCU_COMMAND_ROUTER_H
