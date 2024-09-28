//
// Created by painnick on 2024-09-28.
//

#include "VoiceCommander.h"

VoiceCommander::VoiceCommander(HardwareSerial &serial) : serial(serial) {}

VoiceCommander::~VoiceCommander() {}

void VoiceCommander::begin(unsigned long baud, uint32_t config, int8_t rxPin,
                           int8_t txPin, bool invert, unsigned long timeout_ms,
                           uint8_t rxfifo_full_thrhd) {
  serial.begin(baud, config, rxPin, txPin, invert, timeout_ms,
               rxfifo_full_thrhd);
}

Command VoiceCommander::receive() {
  if (serial.available()) {
    size_t read_bytes = serial.read(buffer, VOICE_COMMANDER_PACKET_SIZE);
    if (read_bytes != VOICE_COMMANDER_PACKET_SIZE) {
      ESP_LOGW(VC_TAG, "%d bytes read", read_bytes);
      return Command::UNKNOWN;
    }

    uint64_t read_data =
        buffer[3] + (buffer[2] << 8) + (buffer[1] << 16) + (buffer[0] << 24);

    ESP_LOGD(VC_TAG, "Read 0x%08X", read_data);

    for (int i = 0; i < COMMAND_COUNT; i++) {
      uint64_t val = Commands[i];
      if (read_data == val) {
        ESP_LOGD(VC_TAG, "Found. [%d](%s)", i, ToString(Commands[i]).c_str());
        return Commands[i];
      }
    }

    ESP_LOGW(VC_TAG, "Not Found.");
  }

  return Command::UNKNOWN;
}
