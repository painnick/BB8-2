#include "WifiLed.h"

#include "esp32-hal-log.h"
#include <Arduino.h>
#include <CircularBuffer.hpp>

#include "pins.h"

#define LED_TAG "LED"

LedMode ledMode{LED_MODE_FIXED};
LedMode lastLedMode{LED_MODE_FIXED};
uint32_t ledDuty;
uint32_t lastLedDuty{};
bool ledChanged = false;
unsigned long lastLedChecked{};

CircularBuffer<LED_ACTION, 10> ledActions;

void internalLedSet(uint32_t val1) {
  ledcWrite(WIFI_LED_PIN, val1);
  ESP_LOGD(LED_TAG, "Set duty %u", val1);
}

void initLed() {
  ledcAttachChannel(WIFI_LED_PIN, 1000, 8, WIFI_LED_CH);
}

bool loopLed(unsigned long now) {
  bool isLastAction = false;

  switch (ledMode) {
    case LedMode::LED_MODE_FIXED:
      ESP_LOGD(LED_TAG, "FIXED");
      if (ledChanged) {
        ESP_LOGD(LED_TAG, "FIXED. Changed");
        internalLedSet(ledDuty);
        ledChanged = false;
      }
      isLastAction = true;
      break;
    case LedMode::LED_MODE_ACTIONS:
      if (ledActions.isEmpty()) {
        if (ledChanged) {
          ESP_LOGD(LED_TAG, "Empty. But Changed");
          internalLedSet(ledDuty);
        }
        ledChanged = false;
      } else {
        LED_ACTION firstAction = ledActions.first();
        if (firstAction.endMs <= now) {
          ESP_LOGD(LED_TAG, "Do Action");
          LED_ACTION newAction = ledActions.shift();
          internalLedSet(newAction.val1);
        }
      }
      isLastAction = ledActions.isEmpty();
      break;
    default:
      ESP_LOGW(LED_TAG, "Unhandled Mode!");
      break;
  }

  return isLastAction;
}

void appendLedAction(LED_ACTION action) {
  ledMode = LedMode::LED_MODE_ACTIONS;

  if (ledActions.isEmpty()) {
    ESP_LOGD(LED_TAG, "Add First Action! %ul %02X", action.endMs, action.val1);
    ledActions.push(action);
    //    internalLedSet(action.val1);
  } else if (ledActions.isFull()) {

    ESP_LOGW(LED_TAG, "Action queue is FULL!");

  } else {
    LED_ACTION firstAction = ledActions.last();
    if (firstAction.endMs >= action.endMs) {

      ESP_LOGW(LED_TAG, "The previous request ends later!");

    } else {
      ledActions.push(action);

      ESP_LOGD(LED_TAG, "Add new Action %ul %02x", action.endMs, action.val1);
    }
  }
}

void clearWifiLed(bool fixed) {
  if (fixed)
    ledMode = LedMode::LED_MODE_FIXED;

  ledDuty = 0;
  ledChanged = true;
}

void blinkWifiLed() {
  ESP_LOGI(LED_TAG, "Blink WIFI LED");

  unsigned long now = millis();

  ledMode = LedMode::LED_MODE_ACTIONS;

  appendLedAction({.endMs = now + 100, .val1 = 15});
  appendLedAction({.endMs = now + 200, .val1 = 23});
  appendLedAction({.endMs = now + 300, .val1 = 31});
  appendLedAction({.endMs = now + 400, .val1 = 15});
  appendLedAction({.endMs = now + 500, .val1 = 0});
  appendLedAction({.endMs = now + 5000, .val1 = 0});
}