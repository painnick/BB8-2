#include "esp_log.h"
#include <Arduino.h>
#include <HardwareSerial.h>

#include "command.h"
#include "controllers/BluetoothController.h"
#include "controllers/Mp3Controller.h"
#include "controllers/VoiceCommander.h"
#include "controllers/ShiftRegisterController.h"
#include "pinmap.h"

#define MAIN_TAG "Main"

#define ALIVE_SOUND_INTERVAL_MS 30000

VoiceCommander vc02(Serial1);
BluetoothController bt;
ShiftRegisterController shiftRegister(SR_DATA_PIN, SR_LATCH_PIN, SR_CLOCK_PIN);

void setup() {
  vc02.begin(115200, SERIAL_8N1, VOICE_COMMAND_RX_PIN, VOICE_COMMAND_TX_PIN);

  bt.begin("BB-8");

  setupSound();
  setDefaultVolume();
  delay(1000 * 3);
  playWelcome();

  shiftRegister.set(0xFF, 0xFF);
}

uint32_t lastAliveSoundChecked = 0;
void loop() {
  Command cmd;

  cmd = vc02.receive();
  if ((cmd & Command::UNKNOWN) != Command::UNKNOWN) {
    ESP_LOGD(MAIN_TAG, "VC Cmd : %s", ToString(cmd).c_str());
    processCommand(bt, cmd);
  }

  cmd = bt.receive();
  if ((cmd & Command::UNKNOWN) != Command::UNKNOWN) {
    ESP_LOGD(MAIN_TAG, "BT Cmd : %s", ToString(cmd).c_str());
    processCommand(bt, cmd);
  }

  auto now = millis();
  if (now - lastAliveSoundChecked > ALIVE_SOUND_INTERVAL_MS) {
    playAlive();
    lastAliveSoundChecked = now;
  }
  dfmp3.loop();
  shiftRegister.loop(now);
}
