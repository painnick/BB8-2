#pragma once

#include <Arduino.h>

#define COMMAND_COUNT 10

class BluetoothController;

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

extern String ToString(const Command &cmd);

static Command Commands[COMMAND_COUNT] = {
    Command::UNKNOWN,
    Command::WAKE_UP,
    Command::TURN_LEFT,
    Command::TURN_RIGHT,
    Command::PLAY_MUSIC,
    Command::FOOL,
    Command::STOP,
    Command::TURN_ON,
    Command::TURN_OFF,
    Command::WHERE_ARE_YOU,
};

extern void processCommand(BluetoothController &bt, Command cmd);