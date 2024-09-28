/* ============================================================
 * VC-02 Module
 *
 * http://voice.ai-thinker.com/
 * SDK Manager : http://voice.ai-thinker.com/#/productManage
 * ============================================================ */

#include <HardwareSerial.h>

#ifndef BODY_MCU_VOICE_COMMANDER_H
#define BODY_MCU_VOICE_COMMANDER_H

#define VOICE_COMMANDER_PACKET_SIZE 4
#define VOICE_COMMANDER_COMMAND_COUNT 10

enum VCCommand : uint64_t {
  UNKNOWN = 0x43434343,
  WAKE_UP = 0x00000001,
  TURN_LEFT = 0x01000100,
  TURN_RIGHT = 0x01000200,
  PLAY_MUSIC = 0x70000000,
  FOOL = 0x44444444,
  STOP = 0x00000000,
  TURN_ON = 0x02000001,
  TURN_OFF = 0x02000002,
  WHERE_ARE_YOU = 0xFFFFFF01,
};

static VCCommand VCCommands[VOICE_COMMANDER_COMMAND_COUNT] = {
    VCCommand::UNKNOWN,       VCCommand::WAKE_UP,    VCCommand::TURN_LEFT,
    VCCommand::TURN_RIGHT,    VCCommand::PLAY_MUSIC, VCCommand::FOOL,
    VCCommand::STOP,          VCCommand::TURN_ON,    VCCommand::TURN_OFF,
    VCCommand::WHERE_ARE_YOU,
};

extern String ToString(VCCommand cmd);

class VoiceCommander {
public:
  uint8_t buffer[VOICE_COMMANDER_PACKET_SIZE];

  explicit VoiceCommander(HardwareSerial &serial);
  ~VoiceCommander();

  void begin(unsigned long baud, uint32_t config = SERIAL_8N1,
             int8_t rxPin = -1, int8_t txPin = -1, bool invert = false,
             unsigned long timeout_ms = 20000UL,
             uint8_t rxfifo_full_thrhd = 112);
  VCCommand receive();

protected:
  HardwareSerial &serial;
};

#endif // BODY_MCU_VOICE_COMMANDER_H
