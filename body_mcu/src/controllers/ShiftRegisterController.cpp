#include "ShiftRegisterController.h"

#include "esp_log.h"

#define RANDOM_INTERVAL_MS 300

ShiftRegisterController::ShiftRegisterController(uint8_t data_pin,
                                                 uint8_t latch_pin,
                                                 uint8_t clock_pin)
    : pin_data(data_pin),
      pin_latch(latch_pin),
      pin_clock(clock_pin),
      value1(0),
      value2(0),
      changed(false) {
  pinMode(pin_data, OUTPUT);
  pinMode(pin_latch, OUTPUT);
  pinMode(pin_clock, OUTPUT);
}

void ShiftRegisterController::loop(unsigned long now, bool forceUpdate) {
  switch (mode) {
    case ShiftRegisterMode::FIXED:
      if (forceUpdate || changed) {
        internalSet(value1, value2);
        changed = false;
      }
      break;
    case ShiftRegisterMode::ACTIONS:
      if (actions.isEmpty()) {
        if (forceUpdate || changed) {
          internalSet(value1, value2);
        }
        changed = false;
      } else {
        SR_ACTION firstAction = actions.first();
        if (now < firstAction.endMs) {
          if (firstAction.idx != runningActionIndex) {
            ESP_LOGD(SR_TAG, "Do First Action");
            runningActionIndex = firstAction.idx;
            internalSet(firstAction.val1, firstAction.val2);
          }
        } else if (firstAction.endMs < now) {
          ESP_LOGD(SR_TAG, "End First Action. And shift.");
          actions.shift();

          if (!actions.isEmpty()) {
            ESP_LOGD(SR_TAG, "Do Next Action");
            SR_ACTION newAction = actions.first();
            runningActionIndex = newAction.idx;
            internalSet(newAction.val1, newAction.val2);
          }
        }
      }
      break;
    case ShiftRegisterMode::RANDOM:
      if (now - lastChecked > RANDOM_INTERVAL_MS) {
        internalSet(random(256), random(256));
        lastChecked = now;
      }
      break;
    default:
      break;
  }
}

void ShiftRegisterController::set(byte newVal1, byte newVal2) {
  actions.clear();

  mode = ShiftRegisterMode::FIXED;

  changed = (value1 != newVal1) || (value2 != newVal2);

  value1 = newVal1;
  value2 = newVal2;
}

/**
 * |Val2| Val1 |
 *  15~8   7~0
 */
void ShiftRegisterController::on(int index) {
  actions.clear();

  mode = ShiftRegisterMode::FIXED;

  if (index < 8) {
    byte newVal1 = value1;
    bitSet(newVal1, index);
    changed = (value1 != newVal1);
    value1 = newVal1;
  } else {
    int newIndex = index - 8;
    byte newVal2 = value2;
    bitSet(newVal2, newIndex);
    changed = (value2 != newVal2);
    value2 = newVal2;
  }
}

void ShiftRegisterController::off(int index) {
  actions.clear();

  mode = ShiftRegisterMode::FIXED;

  if (index < 8) {
    byte newVal1 = value1;
    bitClear(newVal1, index);
    changed = (value1 != newVal1);
    value1 = newVal1;
  } else {
    int newIndex = index - 8;
    byte newVal2 = value2;
    bitClear(newVal2, newIndex);
    changed = (value2 != newVal2);
    value2 = newVal2;
  }
}

void ShiftRegisterController::clear() {
  value1 = 0;
  value2 = 0;
  changed = true;
}

void ShiftRegisterController::append(SR_ACTION action) {
  lastActionIndex = (lastActionIndex + 1) % 100;
  action.idx = lastActionIndex;

  mode = ShiftRegisterMode::ACTIONS;

  if (actions.isEmpty()) {
    ESP_LOGD(SR_TAG, "Add First Action! %ul %02X", action.endMs, action.val1);
    actions.push(action);
    internalSet(action.val1, action.val2);
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

byte ShiftRegisterController::get() const { return value1; }

void ShiftRegisterController::internalSet(byte val1, byte val2) const {
  digitalWrite(pin_latch, LOW);
  shiftOut(pin_data, pin_clock, MSBFIRST, val1);
  shiftOut(pin_data, pin_clock, MSBFIRST, val2);
  digitalWrite(pin_latch, HIGH);

  ESP_LOGD(SR_TAG, "%02X %02X", val1, val2);
}

void ShiftRegisterController::warningMessage() {
  unsigned long now = millis();

  mode = ShiftRegisterMode::ACTIONS;

  append({.endMs = now + 200, .val1 = 0xFF, .val2 = 0xFF});
  append({.endMs = now + 400, .val1 = 0x00, .val2 = 0x00});
  append({.endMs = now + 600, .val1 = 0xFF, .val2 = 0xFF});
  append({.endMs = now + 800, .val1 = 0x00, .val2 = 0x00});
  append({.endMs = now + 1000, .val1 = 0xFF, .val2 = 0xFF});
  append({.endMs = now + 1200, .val1 = 0x00, .val2 = 0x00});
}

void ShiftRegisterController::randomLight(boolean isOn) {
  actions.clear();

  if (isOn) {
    lastMode = mode;
    lastValue1 = value1;
    lastValue2 = value2;
    mode = ShiftRegisterMode::RANDOM;
  } else {
    if (mode == ShiftRegisterMode::RANDOM) {
      mode = lastMode;
      value1 = lastValue1;
      value2 = lastValue2;
      changed = true;
    }
  }
}
