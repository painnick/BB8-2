//
// Created by painnick on 2024-10-03.
//

#ifndef BODY_MCU_SRC_CONTROLLERS_STATELEDCONTROLLER_H_
#define BODY_MCU_SRC_CONTROLLERS_STATELEDCONTROLLER_H_

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

class StateLedController {
 public:
  StateLedController(uint8_t pin, uint8_t ch);
  bool loop(unsigned long now);
  void on();
  void clear(bool fixed);
  void blink();

 protected:
  uint8_t pin;
  uint8_t ch;

  LedMode ledMode{LED_MODE_FIXED};
  LedMode lastLedMode{LED_MODE_FIXED};
  uint32_t ledDuty;
  uint32_t lastLedDuty{};
  bool ledChanged = false;
  unsigned long lastLedChecked{};

  CircularBuffer<LED_ACTION, 10> ledActions;

  void appendAction(LED_ACTION action);
  void internalSet(uint32_t val1);
};


#endif //BODY_MCU_SRC_CONTROLLERS_STATELEDCONTROLLER_H_
