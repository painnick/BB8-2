#pragma once

#include <Arduino.h>
#include <CircularBuffer.hpp>

#define SR_TAG "SR"

typedef struct {
  int idx;
  unsigned long endMs;
  byte val1;
  byte val2;
} SR_ACTION;

enum ShiftRegisterMode {
  FIXED = 0,
  ACTIONS = 1,
  RANDOM = 2,
};

class ShiftRegisterController {
 public:
  ShiftRegisterController(uint8_t data_pin, uint8_t latch_pin, uint8_t clock_pin);

  void loop(unsigned long now, bool forceUpdate = false);

  void set(byte newVal1, byte newVal2, bool forced = true);

  void on(int index);

  void off(int index);

  void clear(bool forced = true);

  byte get() const;

  void append(SR_ACTION action);

  void warningMessage();

  void randomLight(boolean isOn);

 private:
  int lastActionIndex = 0;
  int runningActionIndex = 0;

  uint8_t pin_data;
  uint8_t pin_latch;
  uint8_t pin_clock;

  ShiftRegisterMode mode{FIXED};
  ShiftRegisterMode lastMode{FIXED};
  byte value1;
  byte value2;
  byte lastValue1{};
  byte lastValue2{};
  bool changed;
  unsigned long lastChecked{};

  CircularBuffer<SR_ACTION, 10> actions;

  void internalSet(byte val1, byte val2) const;
};

extern ShiftRegisterController shiftRegister;
