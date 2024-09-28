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

enum Command : uint64_t {
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

static Command Commands[VOICE_COMMANDER_COMMAND_COUNT] = {
    Command::UNKNOWN,       Command::WAKE_UP,    Command::TURN_LEFT,
    Command::TURN_RIGHT,    Command::PLAY_MUSIC, Command::FOOL,
    Command::STOP,          Command::TURN_ON,    Command::TURN_OFF,
    Command::WHERE_ARE_YOU,
};

extern String ToString(Command cmd);

class VoiceCommander {
public:
  uint8_t buffer[VOICE_COMMANDER_PACKET_SIZE];

  explicit VoiceCommander(HardwareSerial &serial);
  ~VoiceCommander();

  void begin(unsigned long baud, uint32_t config = SERIAL_8N1,
             int8_t rxPin = -1, int8_t txPin = -1, bool invert = false,
             unsigned long timeout_ms = 20000UL,
             uint8_t rxfifo_full_thrhd = 112);
  Command receive();

protected:
  HardwareSerial &serial;
};

#endif // BODY_MCU_VOICE_COMMANDER_H
