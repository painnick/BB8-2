//
// Created by painnick on 2024-09-28.
//

#include "VoiceCommander.h"

#define TAG "VC02"

VoiceCommander::VoiceCommander(HardwareSerial &serial) : serial(serial) {}

VoiceCommander::~VoiceCommander() {}

void VoiceCommander::begin(unsigned long baud, uint32_t config, int8_t rxPin,
                           int8_t txPin, bool invert, unsigned long timeout_ms,
                           uint8_t rxfifo_full_thrhd) {
  serial.begin(baud, config, rxPin, txPin, invert, timeout_ms,
               rxfifo_full_thrhd);
}

Command VoiceCommander::receive() {
  size_t read_bytes = 0;
  uint64_t read_data = Command::UNKNOWN;

  if (serial.available()) {
    read_bytes = serial.read(buffer, VOICE_COMMANDER_PACKET_SIZE);
    if (read_bytes != VOICE_COMMANDER_PACKET_SIZE) {
      ESP_LOGW(TAG, "%d bytes read", read_bytes);
      return Command::UNKNOWN;
    }

    read_data =
        buffer[3] + (buffer[2] << 8) + (buffer[1] << 16) + (buffer[0] << 24);

    ESP_LOGD(TAG, "Read 0x%08X", read_data);

    for (int i = 0; i < VOICE_COMMANDER_COMMAND_COUNT; i++) {
      if ((VCCommands[i] & read_data) == read_data) {
        ESP_LOGD(TAG, "Found.");
        return VCCommands[i];
      }
    }

    ESP_LOGW(TAG, "Not Found.");
  }

  return Command::UNKNOWN;
}
