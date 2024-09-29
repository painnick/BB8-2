/* ============================================================
 * VC-02 Module
 *
 * http://voice.ai-thinker.com/
 * SDK Manager : http://voice.ai-thinker.com/#/productManage
 * ============================================================ */

#include <HardwareSerial.h>

#include "../command.h"

#define VC_TAG "VC02"

#ifndef BODY_MCU_VOICE_COMMANDER_H
#define BODY_MCU_VOICE_COMMANDER_H

#define VOICE_COMMANDER_PACKET_SIZE 4

class VoiceCommander {
 public:
  uint8_t buffer[VOICE_COMMANDER_PACKET_SIZE];

  explicit VoiceCommander(HardwareSerial &serial);
  ~VoiceCommander();

  void begin(unsigned long baud,
             uint32_t config = SERIAL_8N1,
             int8_t rxPin = -1,
             int8_t txPin = -1,
             bool invert = false,
             unsigned long timeout_ms = 20000UL,
             uint8_t rxfifo_full_thrhd = 112);
  Command receive();

 protected:
  HardwareSerial &serial;
};

#endif// BODY_MCU_VOICE_COMMANDER_H
