#pragma once

#include <Arduino.h>

#define cmdSerial Serial1

typedef std::function<void(const String &cmd)> CommandCallback;

void commandRouterBegin(CommandCallback cb);
void commandRouterLoop();
void ackCommand(const String &cmd);