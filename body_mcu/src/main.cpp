#include <Arduino.h>
#include <HardwareSerial.h>

#include "esp_log.h"

#include "controllers/VoiceCommander.h"
#include "pinmap.h"

#define MAIN_TAG "Main"

VoiceCommander vc02(Serial1);

void setup() {
  vc02.begin(115200, SERIAL_8N1, VOICE_COMMAND_RX_PIN, VOICE_COMMAND_TX_PIN);
}

void loop() {
  auto command = vc02.receive();
  if(command != Command::UNKNOWN)
    ESP_LOGW(MAIN_TAG, "Cmd : %s", ToString(command).c_str());

  delay(100);
}
