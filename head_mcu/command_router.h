#pragma once

#include <Arduino.h>

#define cmdSerial Serial1

typedef std::function<void(const String &cmd)> CommandCallback;

void commandRouterBegin(CommandCallback cb);
void commandRouterLoop(unsigned long now);
void logMessage(const String &cmd);
void ackCommand(const String &cmd);
void sendCommand(const String &cmd);