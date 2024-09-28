#include <Arduino.h>
#include <HardwareSerial.h>
#include "esp_log.h"

#include "pinmap.h"
#include "controllers/VoiceCommander.h"
#include "controllers/Mp3Controller.h"
#include "controllers/BluetoothController.h"

#define MAIN_TAG "Main"

VoiceCommander vc02(Serial1);
BluetoothController bt;

void setup() {
  vc02.begin(115200, SERIAL_8N1, VOICE_COMMAND_RX_PIN, VOICE_COMMAND_TX_PIN);

  bt.begin("BB-8");

  setupSound();
  setDefaultVolume();
  delay(1000 * 3);
  playWelcome();
}

uint32_t lastChecked = 0;
void loop() {
  auto vcCommand = vc02.receive();
  if(vcCommand != VCCommand::VC_UNKNOWN)
    ESP_LOGW(MAIN_TAG, "VC Cmd : %s", ToString(vcCommand).c_str());

  auto btCommand = bt.receive();
  if(btCommand != BTCommand::BT_UNKNOWN)
    ESP_LOGW(MAIN_TAG, "BT Cmd : %s", ToString(btCommand).c_str());

  auto now = millis();
  if(now - lastChecked > 1000 * 10) {
    playAlive();
    lastChecked = now;
  }
  dfmp3.loop();
}
