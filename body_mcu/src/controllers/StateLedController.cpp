//
// Created by painnick on 2024-10-03.
//

#include "StateLedController.h"
#include "esp32-hal-log.h"
#include <Arduino.h>
#include <CircularBuffer.hpp>

#define LED_TAG "LED"

StateLedController::StateLedController(uint8_t pin, uint8_t ch) : pin(pin), ch(ch) {
  ledcSetup(ch, 1000, 8);
  ledcAttachPin(pin, ch);
}

bool StateLedController::loop(unsigned long now) {
  bool isLastAction = false;

  switch (ledMode) {
    case LedMode::LED_MODE_FIXED:
      if (ledChanged) {
        // ESP_LOGD(LED_TAG, "FIXED. Changed");
        internalSet(ledDuty);
        ledChanged = false;
      }
      isLastAction = true;
      break;
    case LedMode::LED_MODE_ACTIONS:
      if (ledActions.isEmpty()) {
        if (ledChanged) {
          // ESP_LOGD(LED_TAG, "Empty. But Changed");
          internalSet(ledDuty);
        }
        ledChanged = false;
      } else {
        LED_ACTION firstAction = ledActions.first();
        if (firstAction.endMs <= now) {
          // ESP_LOGD(LED_TAG, "Do Action");
          LED_ACTION newAction = ledActions.shift();
          internalSet(newAction.val1);
        }
      }
      isLastAction = ledActions.isEmpty();
      break;
    default:
      // ESP_LOGW(LED_TAG, "Unhandled Mode!");
      break;
  }

  return isLastAction;
}

void StateLedController::internalSet(uint32_t val1) {
  ledcWrite(ch, val1);
  // ESP_LOGD(LED_TAG, "Set duty %u", val1);
}

void StateLedController::appendAction(LED_ACTION action) {
  ledMode = LedMode::LED_MODE_ACTIONS;

  if (ledActions.isEmpty()) {
    // ESP_LOGD(LED_TAG, "Add First Action! %ul %02X", action.endMs, action.val1);
    ledActions.push(action);
    //    internalLedSet(action.val1);
  } else if (ledActions.isFull()) {

    // ESP_LOGW(LED_TAG, "Action queue is FULL!");

  } else {
    LED_ACTION firstAction = ledActions.last();
    if (firstAction.endMs >= action.endMs) {

      // ESP_LOGW(LED_TAG, "The previous request ends later!");

    } else {
      ledActions.push(action);

      // ESP_LOGD(LED_TAG, "Add new Action %ul %02x", action.endMs, action.val1);
    }
  }
}

void StateLedController::on() {
  ledMode = LedMode::LED_MODE_FIXED;
  ledDuty = 255;
  ledChanged = true;
}

void StateLedController::clear(bool fixed) {
  if (fixed)
    ledMode = LedMode::LED_MODE_FIXED;

  ledDuty = 0;
  ledChanged = true;
}

void StateLedController::blink() {

  unsigned long now = millis();

  ledMode = LedMode::LED_MODE_ACTIONS;

  appendAction({.endMs = now + 120, .val1 = 15});
  appendAction({.endMs = now + 240, .val1 = 19}); // + 4
  appendAction({.endMs = now + 360, .val1 = 23});
  appendAction({.endMs = now + 480, .val1 = 27});
  appendAction({.endMs = now + 600, .val1 = 31});
  // HOLD to 720

  appendAction({.endMs = now + 800, .val1 = 26}); // - 5
  appendAction({.endMs = now + 900, .val1 = 21});
  appendAction({.endMs = now + 1000, .val1 = 16});

  appendAction({.endMs = now + 1100, .val1 = 0});
  appendAction({.endMs = now + 5000, .val1 = 0});
}