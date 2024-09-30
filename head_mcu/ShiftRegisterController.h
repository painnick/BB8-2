#pragma once

#include <Arduino.h>
#include <CircularBuffer.hpp>

#define SR_TAG "SR"

typedef struct {
  unsigned long endMs;
  byte val1;
  byte val2;
} SR_ACTION;

enum ShiftRegisterMode {
  FIXED = 0,
  ACTIONS = 1,
  RANDOM = 2,
};

void initShiftRegister();
void loopShiftRegister(unsigned long now, bool forceUpdate = false);
void setShiftRegister(byte newVal1);
void shiftRegisterOn(int index);
void shiftRegisterOff(int index);
void clearShiftRegister(bool fixed = true);
void appendShiftRegisterAction(SR_ACTION action);
byte getShiftRegisterValue();
void warningMessage();
void randomLight(boolean isOn);
