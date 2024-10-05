#pragma once

#include <Arduino.h>

typedef struct {
  int idx;
  unsigned long endMs;
  byte val1;
} SR_ACTION;

enum ShiftRegisterMode {
  FIXED = 0,
  ACTIONS = 1,
  RANDOM = 2,
};

void initShiftRegister();
bool loopShiftRegister(unsigned long now, bool forceUpdate = false);
void setShiftRegister(byte newVal1);
void shiftRegisterOn(int index);
void shiftRegisterOff(int index);
void clearShiftRegister(bool fixed = true);
void appendShiftRegisterAction(SR_ACTION action);
byte getShiftRegisterValue();
void warningMessage();
void randomLight(boolean isOn);
