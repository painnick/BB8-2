#include "esp32-hal-log.h"
#include <Arduino.h>

#include "ShiftRegisterController.h"
#include "WifiLED.h"
#include "ap_control.h"
#include "app_httpd.h"
#include "camera_control.h"
#include "command_router.h"

void process(const String &cmd);

void setup() {
  initLed();

  initShiftRegister();
  setShiftRegister(0xFF);

  initCamera();

  commandRouterBegin(process);
}

void loop() {
  auto now = millis();
  auto isEnd = loopLed(now);
  if (isEnd) {
    log_w("NO MORE LED Action!");
    delay(1000);
  }
  if (isEnd && isAPOn()) {
    log_d("Recall Blink LED");
    blinkWifiLed();
  }
  commandRouterLoop(now);
  loopShiftRegister(now);
}

void process(const String &cmd) {
  if (cmd == "WIFION") {
    if (!isAPOn()) {
      initSoftAP();
      startCameraServer();
      blinkWifiLed();
    }
    ackCommand(cmd);
  } else if (cmd == "WIFIOFF") {
    if (isAPOn()) {
      stopCameraServer();
      closeSoftAP();
      clearWifiLed(true);
    }
    ackCommand(cmd);
  } else if (cmd == "WARN") {
    warningMessage();
    ackCommand(cmd);
  } else if (cmd == "RANDOM1") {
    randomLight(true);
    ackCommand(cmd);
  } else if (cmd == "RANDOM2") {
    randomLight(false);
    ackCommand(cmd);
  } else if (cmd == "LIGHTOFF") {
    clearShiftRegister();
    ackCommand(cmd);
  } else {
    ackCommand("Unknown");
    log_w("Unhandled command : %s", cmd);
  }
}
