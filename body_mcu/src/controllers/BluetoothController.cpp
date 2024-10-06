//
// Created by painnick on 2024-09-28.
//

#include "BluetoothController.h"
#include "../../../head_mcu/message.h"

#include <utility>

#define BT_TAG "BLUETOOTH"

#define CMD_DFPLAYER_VOLUME "DFVOL"

static const BluetoothCommandInfo btCommands[] = {
    {BT_UNKNOWN, CMD_UNKNOWN, "Unknown"},
    {BT_HELP, "HELP", "Help"},
    {BT_ACK, CMD_ACK, "Ack"},
    {BT_WAKE_UP, "WAKEUP", "WakeUp"},
    {BT_SLEEP, "SLEEP", "Sleep"},
    {BT_STOP, "STOP", "Stop"},
    {BT_TURN_LEFT, CMD_TURN_LEFT, "TurnLeft"},
    {BT_TURN_RIGHT, CMD_TURN_RIGHT, "TurnRight"},
    {BT_LIGHT_ON, CMD_LIGHT_ON, "LightOn"},
    {BT_LIGHT_OFF, CMD_LIGHT_OFF, "LightOff"},
    {BT_WIFI_ON, CMD_WIFI_ON, "WiFiOn"},
    {BT_WIFI_OFF, CMD_WIFI_OFF, "WiFiOff"},
    {BT_PLAY_MUSIC, "MUSIC", "PlayMusic"},
    {BT_FOOL, "FOOL", "Fool"},
    {BT_LOOK_AT_ME, "LOOK", "LookAtMe"},
    {BT_ATTENTION, "ATTENTION", "Attention"},
    {BT_DFPLAYER_VOLUME, CMD_DFPLAYER_VOLUME, CMD_DFPLAYER_VOLUME " %d(1~30)"},
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
  if (msg.indexOf(CMD_ACK) == 0) {
    return BT_ACK;
  }
  if (msg.indexOf(CMD_DFPLAYER_VOLUME) == 0) {
    return BT_DFPLAYER_VOLUME;
  }
  for (const auto &btCommand : btCommands) {
    if (msg.indexOf(btCommand.msg) != -1)
      return btCommand.commandType;
  }
  return BT_UNKNOWN;
}

void BluetoothController::init(BluetoothControllerCallback callback) {
  proc = std::move(callback);
}

void BluetoothController::begin() {
  if (isBluetoothOn)
    return;

  serial.begin(name);
  serial.register_callback([](esp_spp_cb_event_t evt, esp_spp_cb_param_t *param) {
    switch (evt) {
      case ESP_SPP_SRV_OPEN_EVT:
        // TODO. Send Command list
        break;
      default:
        break;
    }
  });
  isBluetoothOn = true;
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
  ESP_LOGD(BT_TAG, "Print help messages");
  serial.println("Commands");
  for (const auto &btCommand : btCommands) {
    serial.print("- ");
    serial.print(btCommand.msg.c_str());
    serial.print(" : ");
    serial.println(btCommand.desc.c_str());
  }
  serial.flush();
}

void BluetoothController::close() {
  if (!isBluetoothOn)
    return;

  serial.flush();
  serial.disconnect();
  serial.end();

  isBluetoothOn = false;
}