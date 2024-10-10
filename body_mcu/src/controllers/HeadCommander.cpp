//
// Created by painnick on 2024-09-28.
//

#include "HeadCommander.h"
#include "../../../head_mcu/message.h"

#define COMMAND_DELIMITER "/|"
#define COMMAND_DELIMITER_SIZE 2
#define MAX_COMMAND_BUFFER_SIZE 50

static const HeadCommandInfo headCommands[] = {
    {HEAD_UNKNOWN, CMD_UNKNOWN, "Unknown"},
    {HEAD_ACK, CMD_ACK, "Ack"},
    {HEAD_LOG, CMD_LOGMSG, "Log"},
    {HEAD_WIFI_ON, CMD_WIFI_ON, "WiFiOn"},
    {HEAD_WIFI_OFF, CMD_WIFI_OFF, "WiFiOff"},
    {HEAD_FOOL, CMD_FOOL, "Fool"},
    {HEAD_LIGHT_ON, CMD_LIGHT_ON, "LightOn"},
    {HEAD_LIGHT_OFF, CMD_LIGHT_OFF, "LightOff"},
    {HEAD_TURN_LEFT, CMD_TURN_LEFT, "TurnLeft"},
    {HEAD_TURN_RIGHT, CMD_TURN_RIGHT, "TurnRight"},
    {HEAD_TURN_LEFT2, CMD_TURN_LEFT2, "TurnLeft2"},
    {HEAD_TURN_RIGHT2, CMD_TURN_RIGHT2, "TurnRight2"},
    {HEAD_STOP_NOW, CMD_STOP_NOW, "StopNow"},
    {HEAD_WIFI_IS_ON, CMD_WIFI_IS_ON, "WiFiIsOn"},
    {HEAD_WIFI_IS_OFF, CMD_WIFI_IS_OFF, "WiFiIsOff"},
    {HEAD_BT_ON, CMD_BLUETOOTH_ON, "BluetoothOn"},
    {HEAD_BT_OFF, CMD_BLUETOOTH_OFF, "BluetoothOff"},};

String ToString(const HeadCommandInfo &cmd) {
  for (const auto &headCommand : headCommands) {
    if (cmd.commandType == headCommand.commandType)
      return cmd.desc;
  }
  return headCommands[0].desc;
}

String ToMessage(HeadCommandType cmdType) {
  for (const auto &headCommand : headCommands) {
    if (cmdType == headCommand.commandType)
      return headCommand.msg;
  }
  return headCommands[0].msg;
}

HeadCommandType ToHeadCommandType(String &msg) {
  if (msg.indexOf(CMD_ACK) == 0) {
    return HEAD_ACK;
  }

  if (msg.indexOf(CMD_LOGMSG) == 0) {
    return HEAD_LOG;
  }

  for (const auto &headCommand : headCommands) {
    if (msg == headCommand.msg)
      return headCommand.commandType;
  }
  return HEAD_UNKNOWN;
}

HeadCommander::HeadCommander(SoftwareSerial &serial) : serial(serial) {}

HeadCommander::~HeadCommander() = default;

void HeadCommander::init(HeadCommanderCallback callback) {
  proc = std::move(callback);
}

void HeadCommander::begin(uint32_t baud, EspSoftwareSerial::Config config, int8_t rxPin, int8_t txPin) {
  serial.begin(baud, config, rxPin, txPin);
}

void HeadCommander::loop() {
  if (serial.available()) {
    // Append command-buffer
    while (serial.available()) {
      cmdBuffer += (char) serial.read();
    }
    // Check size of command-buffer
    if (cmdBuffer.length() > MAX_COMMAND_BUFFER_SIZE) {
      cmdBuffer = "";
    } else {
      while (-1 != cmdBuffer.indexOf(COMMAND_DELIMITER)) {
        int found = cmdBuffer.indexOf(COMMAND_DELIMITER);
        if (found != -1) {
          String cmd = cmdBuffer.substring(0, found);
          cmdBuffer = cmdBuffer.substring(found + COMMAND_DELIMITER_SIZE);
          ESP_LOGV(HEAD_TAG, "(HEAD) <= %s", cmd.c_str());

          auto cmdType = ToHeadCommandType(cmd);

          if(cmdType == HEAD_WIFI_IS_ON) {
            isWifiOn = true;
          } else if(cmdType == HEAD_WIFI_IS_OFF) {
            isWifiOn = false;
          }
          proc(this, cmdType);
        }
      }
    }
  }
}

void HeadCommander::send(HeadCommandType cmd) {
  auto msg = ToMessage(cmd);
  serial.printf("%s", msg.c_str());
  serial.printf(COMMAND_DELIMITER);
  serial.flush();

  ESP_LOGD(HEAD_TAG, "(BODY) => %s", msg.c_str());
}