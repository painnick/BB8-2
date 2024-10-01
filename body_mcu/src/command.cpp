#include "command.h"

#include "./controllers/BluetoothController.h"

String ToString(const Command &cmd) {
  switch (cmd) {
    case Command::UNKNOWN:
      return "Unknown";
    case Command::WAKE_UP:
      return "WAKE_UP";
    case Command::BYE:
      return "BYE";
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

    case Command::HEAD_MOVE_LEFT:
      return "HEAD_MOVE_LEFT";
    case Command::HEAD_MOVE_RIGHT:
      return "HEAD_MOVE_RIGHT";
    case Command::HEAD_MOVE_STOP:
      return "HEAD_MOVE_STOP";
    case Command::HEAD_MOVE_OPPOSITE:
      return "HEAD_MOVE_OPPOSITE";
    case Command::HEAD_MOVE_RANDOM:
      return "HEAD_MOVE_RANDOM";
    default:
      return "Not found";
  }
}

Command ToCommand(const String&cmd) {
  if (cmd == "wake_up")
    return Command::WAKE_UP;
  else if (cmd == "bye")
    return Command::BYE;
  else if (cmd == "turn_left")
    return Command::TURN_LEFT;
  else if (cmd == "turn_right")
    return Command::TURN_RIGHT;
  else if (cmd == "play_music")
    return Command::PLAY_MUSIC;
  else if (cmd == "fool")
    return Command::FOOL;
  else if (cmd == "stop")
    return Command::STOP;
  else if (cmd == "turn_on")
    return Command::TURN_ON;
  else if (cmd == "turn_off")
    return Command::TURN_OFF;
  else if (cmd == "where")
    return Command::WHERE_ARE_YOU;
  else if (cmd == "head_move_left")
    return Command::HEAD_MOVE_LEFT;
  else if (cmd == "head_move_right")
    return Command::HEAD_MOVE_RIGHT;
  else if (cmd == "head_move_stop")
    return Command::HEAD_MOVE_STOP;
  else if (cmd == "head_move_opposite")
    return Command::HEAD_MOVE_OPPOSITE;
  else if (cmd == "head_move_random")
    return Command::HEAD_MOVE_RANDOM;
  else
    return Command::UNKNOWN;
}
