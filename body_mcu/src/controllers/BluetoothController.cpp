//
// Created by painnick on 2024-09-28.
//

#include "BluetoothController.h"
#include "command.h"

#include <utility>

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

Command BluetoothController::receive() {
  if (serial.available()) {
    String btCmd = serial.readString();
    ESP_LOGD(BT_TAG, "BT >> %s", btCmd.c_str());

    return ToCommand(btCmd);
  }
  return Command::UNKNOWN;
}

void BluetoothController::println(const char c[]) { serial.println(c); }