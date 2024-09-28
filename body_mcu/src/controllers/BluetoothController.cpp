//
// Created by painnick on 2024-09-28.
//

#include "BluetoothController.h"

#define BT_TAG "BLUETOOTH"

String ToString(BTCommand cmd) {
  switch (cmd) {
  case BTCommand::BT_UNKNOWN:
    return "Unknown";
  case BTCommand::BT_WAKE_UP:
    return "BT_WAKE_UP";
  case BTCommand::BT_TURN_LEFT:
    return "BT_TURN_LEFT";
  case BTCommand::BT_TURN_RIGHT:
    return "BT_TURN_RIGHT";
  case BTCommand::BT_PLAY_MUSIC:
    return "BT_PLAY_MUSIC";
  case BTCommand::BT_FOOL:
    return "BT_FOOL";
  case BTCommand::BT_STOP:
    return "BT_STOP";
  case BTCommand::BT_TURN_ON:
    return "BT_TURN_ON";
  case BTCommand::BT_TURN_OFF:
    return "BT_TURN_OFF";
  case BTCommand::BT_WHERE_ARE_YOU:
    return "BT_WHERE_ARE_YOU";
  default:
    return "Not found";
  }
}

void BluetoothController::begin(String name) {
  serial.begin(name);
  serial.register_callback(
      [](esp_spp_cb_event_t evt, esp_spp_cb_param_t *param) {
        switch (evt) {
        case ESP_SPP_SRV_OPEN_EVT:
          break;
        default:
          break;
        }
      });
}

BTCommand BluetoothController::receive() {
  if (serial.available()) {
    String btCmd = serial.readString();
    ESP_LOGD(BT_TAG, "BT >> %s", btCmd.c_str());

    if (btCmd == "wake_up")
      return BTCommand::BT_WAKE_UP;
    else if (btCmd == "turn_left")
      return BTCommand::BT_TURN_LEFT;
    else if (btCmd == "turn_right")
      return BTCommand::BT_TURN_RIGHT;
    else if (btCmd == "play_music")
      return BTCommand::BT_PLAY_MUSIC;
    else if (btCmd == "fool")
      return BTCommand::BT_FOOL;
    else if (btCmd == "stop")
      return BTCommand::BT_STOP;
    else if (btCmd == "turn_on")
      return BTCommand::BT_TURN_ON;
    else if (btCmd == "turn_off")
      return BTCommand::BT_TURN_OFF;
    else if (btCmd == "where")
      return BTCommand::BT_WHERE_ARE_YOU;
  }
  return BTCommand::BT_UNKNOWN;
}
