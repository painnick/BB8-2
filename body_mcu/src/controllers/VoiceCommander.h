/* ============================================================
 * VC-02 Module
 *
 * http://voice.ai-thinker.com/
 * SDK Manager : http://voice.ai-thinker.com/#/productManage
 * ============================================================ */

#include <Arduino.h>
#include <HardwareSerial.h>

#ifndef BODY_MCU_VOICE_COMMANDER_H
#define BODY_MCU_VOICE_COMMANDER_H

class VoiceCommander;

enum VoiceCommandType {
  VC02_UNKNOWN,
  VC02_WAKE_UP,
  VC02_SLEEP,
  VC02_STOP,
  VC02_KEEPALIVE,
  VC02_TURN_LEFT,
  VC02_TURN_RIGHT,
  VC02_TURN_ON_LIGHT,
  VC02_TURN_OFF_LIGHT,
  VC02_TURN_ON_AP,
  VC02_TURN_OFF_AP,
  VC02_TURN_ON_BLUETOOTH,
  VC02_TURN_OFF_BLUETOOTH,
  VC02_PLAY_MUSIC,
  VC02_FOOL,
  VC02_LOOK_AT_ME,
  VC02_ATTENTION,
};

typedef struct {
  VoiceCommandType commandType;
  byte buffer;
  String desc;
} VoiceCommand;

String ToString(const VoiceCommand &cmd);

VoiceCommandType ToVoiceCommandType(char buffer);

typedef std::function<void(const VoiceCommander *, const VoiceCommandType cmdType)> VoiceCommanderCallback;

class VoiceCommander {
 public:
  explicit VoiceCommander(HardwareSerial &serial);

  ~VoiceCommander();

  void init(VoiceCommanderCallback callback);

  void begin(unsigned long baud,
             uint32_t config = SERIAL_8N1,
             int8_t rxPin = -1,
             int8_t txPin = -1,
             bool invert = false,
             unsigned long timeout_ms = 20000UL,
             uint8_t rxfifo_full_thrhd = 112);

  void loop();

  void send(byte val);

  bool isListening = true;

 protected:
  HardwareSerial &serial;
  VoiceCommanderCallback proc;
};

#endif// BODY_MCU_VOICE_COMMANDER_H
