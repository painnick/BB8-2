//
// Created by painnick on 2024-09-28.
//

#include "VoiceCommander.h"

#include <utility>

#define VC_TAG "VC02"

static const VoiceCommand voiceCommands[] = {
    {VC02_UNKNOWN, 0x00, "Unknown"},
    {VC02_WAKE_UP, 0x01, "WakeUp"},
    {VC02_SLEEP, 0x02, "Sleep"},
    {VC02_STOP, 0x03, "Stop"},
    {VC02_KEEPALIVE, 0x04, "KeepAlive"},

    {VC02_TURN_LEFT, 0x11, "HeadLeft"},
    {VC02_TURN_RIGHT, 0x12, "HeadRight"},

    {VC02_TURN_ON_LIGHT, 0x41, "LightOn"},
    {VC02_TURN_OFF_LIGHT, 0x42, "LightOff"},

    {VC02_TURN_ON_AP, 0x51, "WifiOn"},
    {VC02_TURN_OFF_AP, 0x52, "WifiOff"},
    {VC02_TURN_ON_BLUETOOTH, 0x53, "BluetoothOn"},
    {VC02_TURN_OFF_BLUETOOTH, 0x53, "BluetoothOff"},

    {VC02_PLAY_MUSIC, 0x21, "PlayMusic"},
    {VC02_VOLUME, 0x22, "Volume"},

    {VC02_FOOL, 0x31, "Fool"},
    {VC02_LOOK_AT_ME, 0x32, "LookAtMe"},
    {VC02_ATTENTION, 0x33, "Attention"},
};

String ToString(const VoiceCommand &cmd) {
  for (const auto &voiceCommand : voiceCommands) {
    if (cmd.commandType == voiceCommand.commandType)
      return cmd.desc;
  }
  return voiceCommands[0].desc;
}

VoiceCommandType ToVoiceCommandType(char buffer) {
  for (const auto &voiceCommand : voiceCommands) {
    if (buffer == voiceCommand.buffer)
      return voiceCommand.commandType;
  }
  return voiceCommands[0].commandType;
}

VoiceCommander::VoiceCommander(HardwareSerial &serial) : serial(serial) {}

VoiceCommander::~VoiceCommander() = default;

void VoiceCommander::init(VoiceCommanderCallback callback) {
  proc = std::move(callback);
}

void VoiceCommander::begin(unsigned long baud,
                           uint32_t config,
                           int8_t rxPin,
                           int8_t txPin,
                           bool invert,
                           unsigned long timeout_ms,
                           uint8_t rxfifo_full_thrhd) {
  serial.begin(baud, config, rxPin, txPin, invert, timeout_ms, rxfifo_full_thrhd);
}

void VoiceCommander::loop() {
  if (serial.available()) {
    char buffer[1];
    size_t read_bytes = serial.read(buffer, 1);
    if (read_bytes != 1) {
      ESP_LOGW(VC_TAG, "%d bytes read", read_bytes);
      return;
    }
    ESP_LOGD(VC_TAG, "Read 0x%02X", buffer[0]);

    auto commandType = ToVoiceCommandType(buffer[0]);
    if (commandType == VC02_WAKE_UP)
      isListening = true;
    else if (commandType == VC02_SLEEP)
      isListening = false;
    proc(this, commandType);
  }
}

void VoiceCommander::send(byte val) {
  char buffer[1];
  buffer[0] = val;
  serial.write(buffer, 1);
  serial.flush();
  ESP_LOGD(VC_TAG, "Send 0x%02X", val);
}

void VoiceCommander::send(byte *val, int len) {
  serial.write(val, len);
  serial.flush();

  if (len == 1)
    ESP_LOGD(VC_TAG, "Send 0x%02X", val[0]);
  else
    ESP_LOGD(VC_TAG, "Send 0x%02X 0x%02X", val[0], val[1]);
}