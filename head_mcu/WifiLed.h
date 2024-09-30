#pragma once

#include <Arduino.h>
#include <CircularBuffer.hpp>

typedef struct {
  unsigned long endMs;
  byte val1;
} LED_ACTION;

enum LedMode {
  LED_MODE_FIXED = 0,
  LED_MODE_ACTIONS = 1,
};

void initLed();
bool loopLed(unsigned long now);
void appendLedAction(LED_ACTION action);
void clearWifiLed(bool fixed);
void blinkWifiLed();