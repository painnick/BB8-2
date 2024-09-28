//
// Created by painnick on 2024-09-28.
//

#include "BluetoothController.h"

#define TAG "BLUETOOTH"

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

Command BluetoothController::receive() {
  if (serial.available()) {
    String btCmd = serial.readString();
    ESP_LOGD(TAG, "BT >> %s", btCmd.c_str());

    if (btCmd == "wake_up")
      return Command::WAKE_UP;
    else if (btCmd == "turn_left")
      return Command::TURN_LEFT;
    else if (btCmd == "turn_right")
      return Command::TURN_RIGHT;
    else if (btCmd == "play_music")
      return Command::PLAY_MUSIC;
    else if (btCmd == "fool")
      return Command::FOOL;
    else if (btCmd == "stop")
      return Command::STOP;
    else if (btCmd == "turn_on")
      return Command::TURN_ON;
    else if (btCmd == "turn_off")
      return Command::TURN_OFF;
    else if (btCmd == "where")
      return Command::WHERE_ARE_YOU;
  }
  return Command::UNKNOWN;
}

void BluetoothController::println(const char c[]) {
  serial.println(c);
}