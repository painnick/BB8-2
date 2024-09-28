#include "command.h"

String ToString(Command cmd) {
  switch (cmd) {
  case Command::UNKNOWN:
    return "Unknown";
  case Command::WAKE_UP:
    return "WAKE_UP";
  case Command::TURN_LEFT:
    return "TURN_LEFT";
  case Command::TURN_RIGHT:
    return "TURN_RIGHT";
  case Command::PLAY_MUSIC:
    return "PLAY_MUSIC";
  case Command::FOOL:
    return "FOOL";
  case Command::STOP:
    return "STOP";
  case Command::TURN_ON:
    return "TURN_ON";
  case Command::TURN_OFF:
    return "TURN_OFF";
  case Command::WHERE_ARE_YOU:
    return "WHERE_ARE_YOU";
  default:
    return "Not found";
  }
}