#include "ShiftRegisterController.h"

#include "esp_log.h"
#include <Arduino.h>
#include <CircularBuffer.hpp>

#include "pins.h"

#define SR_TAG "SR"

#define RANDOM_INTERVAL_MS 300

ShiftRegisterMode mode{FIXED};
ShiftRegisterMode lastMode{FIXED};
byte value1;
byte lastValue1{};
bool changed = false;
unsigned long lastChecked{};

CircularBuffer<SR_ACTION, 10> actions;

void internalSet(byte val1) {
  digitalWrite(SR_LATCH_PIN, LOW);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, val1);
  digitalWrite(SR_LATCH_PIN, HIGH);

  ESP_LOGD(SR_TAG, "%02X", val1);
}

void initShiftRegister() {
  pinMode(SR_DATA_PIN, OUTPUT);
  pinMode(SR_LATCH_PIN, OUTPUT);
  pinMode(SR_CLOCK_PIN, OUTPUT);
}

void loopShiftRegister(unsigned long now, bool forceUpdate) {
  switch (mode) {
    case ShiftRegisterMode::FIXED:
      if (forceUpdate || changed) {
        internalSet(value1);
        changed = false;
      }
      break;
    case ShiftRegisterMode::ACTIONS:
      if (actions.isEmpty()) {
        if (forceUpdate || changed) {
          internalSet(value1);
        }
        changed = false;
      } else {
        SR_ACTION lastAction = actions.first();
        if (lastAction.endMs > now) {
          ESP_LOGD(SR_TAG, "Shift First Action");
          actions.shift();

          if (!actions.isEmpty()) {
            ESP_LOGD(SR_TAG, "Next Action");
            SR_ACTION newAction = actions.first();
            internalSet(newAction.val1);
          }
        }
      }
      break;
    case ShiftRegisterMode::RANDOM:
      if (now - lastChecked > RANDOM_INTERVAL_MS) {
        internalSet(random(256));
        lastChecked = now;
      }
      break;
    default:
      break;
  }
}

void setShiftRegister(byte newVal1) {
  actions.clear();

  mode = ShiftRegisterMode::FIXED;

  changed = (value1 != newVal1);

  value1 = newVal1;
}

void shiftRegisterOn(int index) {
  actions.clear();

  mode = ShiftRegisterMode::FIXED;

  byte newVal1 = value1;
  bitSet(newVal1, index);
  changed = (value1 != newVal1);
  value1 = newVal1;
}

void shiftRegisterOff(int index) {
  actions.clear();

  mode = ShiftRegisterMode::FIXED;

  byte newVal1 = value1;
  bitClear(newVal1, index);
  changed = (value1 != newVal1);
  value1 = newVal1;
}

void clearShiftRegister(bool fixed) {
  if (fixed)
    mode = ShiftRegisterMode::FIXED;

  value1 = 0;
  changed = true;
}

void appendShiftRegisterAction(SR_ACTION action) {
  mode = ShiftRegisterMode::ACTIONS;

  if (actions.isEmpty()) {
    ESP_LOGD(SR_TAG, "Add First Action! %ul %02X", action.endMs, action.val1);
    actions.push(action);
    internalSet(action.val1);
  } else if (actions.isFull()) {

    ESP_LOGW(SR_TAG, "Action queue is FULL!");

  } else {
    SR_ACTION lastAction = actions.last();
    if (lastAction.endMs >= action.endMs) {

      ESP_LOGW(SR_TAG, "The previous request ends later!");

    } else {
      actions.push(action);

      ESP_LOGD(SR_TAG, "Add new Action %ul %02x", action.endMs, action.val1);
    }
  }
}

byte getShiftRegisterValue() { return value1; }

void warningMessage() {
  unsigned long now = millis();

  mode = ShiftRegisterMode::ACTIONS;

  appendShiftRegisterAction({.endMs = now + 300, .val1 = 0xFF});
  appendShiftRegisterAction({.endMs = now + 600, .val1 = 0x00});
  appendShiftRegisterAction({.endMs = now + 900, .val1 = 0xFF});
  appendShiftRegisterAction({.endMs = now + 1200, .val1 = 0x00});
}

void randomLight(boolean isOn) {
  actions.clear();

  if (isOn) {
    lastMode = mode;
    lastValue1 = value1;
    mode = ShiftRegisterMode::RANDOM;
  } else {
    if (mode == ShiftRegisterMode::RANDOM) {
      mode = lastMode;
      value1 = lastValue1;
      changed = true;
    }
  }
}
