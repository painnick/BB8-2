//
// Created by painnick on 2024-09-28.
//

#include "HeadCommander.h"

#define COMMAND_DELIMITER "/|"
#define COMMAND_DELIMITER_SIZE 2
#define MAX_COMMAND_BUFFER_SIZE 50

static const HeadCommandInfo headCommands[] = {
    {HEAD_UNKNOWN, "UNKNOWN", "Unknown"},
    {HEAD_ACK, "ACK", "Ack"},
    {HEAD_LOG, "LOG", "Log"},
    {HEAD_WIFI_ON, "WIFION", "WiFiOn"},
    {HEAD_WIFI_OFF, "WIFIOFF", "WiFiOff"},
    {HEAD_WARN, "WARN", "Warn"},
    {HEAD_RANDOM_LIGHT1, "RANDOM1", "RandomLight1"},
    {HEAD_RANDOM_LIGHT2, "RANDOM2", "RandomLight2"},
    {HEAD_LIGHT_ON, "LIGHTON", "LightOn"},
    {HEAD_LIGHT_OFF, "LIGHTOFF", "LightOff"},
    {HEAD_TURN_LEFT, "TURNLEFT", "TurnLeft"},
    {HEAD_TURN_RIGHT, "TURNRIGHT", "TurnRight"},
    {HEAD_WIFI_IS_ON, "WIFIISON", "WiFiIsOn"},
    {HEAD_WIFI_IS_OFF, "WIFIISOFF", "WiFiIsOff"},
};

static const String logMessages[] = {
    "INITCMD",
    "SETUP",
    "BUFFULL"
};

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
  for (const auto &logMsg : logMessages) {
    if (msg == logMsg)
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