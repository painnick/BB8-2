//
// Created by painnick on 2024-09-28.
//

#include "BluetoothController.h"

#include <utility>

#define BT_TAG "BLUETOOTH"

static const BluetoothCommandInfo btCommands[] = {
    {BT_UNKNOWN, "UNKNOWN", "Unknown"},
    {BT_ACK, "ACK", "Ack"},
    {BT_WIFI_ON, "WIFION", "WiFiOn"},
    {BT_WIFI_OFF, "WIFIOFF", "WiFiOff"},
    {BT_WARN, "WARN", "Warn"},
    {BT_RANDOM_LIGHT1, "RANDOM1", "RandomLight1"},
    {BT_RANDOM_LIGHT2, "RANDOM2", "RandomLight2"},
    {BT_LIGHT_ON, "LIGHTON", "LightOn"},
    {BT_LIGHT_OFF, "LIGHTOFF", "LightOff"},
    {BT_TURN_LEFT, "TURNLEFT", "TurnLeft"},
    {BT_TURN_RIGHT, "TURNRIGHT", "TurnRight"},
};

String ToString(const BluetoothCommandInfo &cmd) {
  for (const auto &btCommand : btCommands) {
    if (cmd.commandType == btCommand.commandType)
      return cmd.desc;
  }
  return btCommands[0].desc;
}

String ToMessage(BluetoothCommandType cmdType) {
  for (const auto &btCommand : btCommands) {
    if (cmdType == btCommand.commandType)
      return btCommand.msg;
  }
  return btCommands[0].msg;
}

BluetoothCommandType ToBluetoothCommandType(String &msg) {
  for (const auto &btCommand : btCommands) {
    if (msg.indexOf(btCommand.msg) != -1)
      return btCommand.commandType;
  }
  return BT_UNKNOWN;
}

void BluetoothController::init(BluetoothControllerCallback callback) {
  proc = std::move(callback);
}

void BluetoothController::begin(String name) {
  serial.begin(std::move(name));
  serial.register_callback([](esp_spp_cb_event_t evt, esp_spp_cb_param_t *param) {
    switch (evt) {
      case ESP_SPP_SRV_OPEN_EVT:
        // TODO. Send Command list
        break;
      default:
        break;
    }
  });
}

void BluetoothController::loop() {
  if (serial.available()) {
    String btCmd = serial.readString();
    ESP_LOGD(BT_TAG, "BT >> %s", btCmd.c_str());

    auto cmdType = ToBluetoothCommandType(btCmd);
    proc(this, cmdType, btCmd);
  }
}

void BluetoothController::println(const char c[]) {
  serial.println(c);
  ESP_LOGD(BT_TAG, "BT << %s", c);
}

void BluetoothController::printHelp() {
  // Do not log
  serial.println("Commands");
  for (const auto &btCommand : btCommands) {
    serial.print("- ");
    serial.println(btCommand.msg);
  }
}