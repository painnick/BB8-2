#include "esp32-hal-log.h"
#include <Arduino.h>
#include "message.h"

#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems

#include "ShiftRegisterController.h"
#include "WifiLED.h"
#include "ap_control.h"
#include "app_httpd.h"
#include "camera_control.h"
#include "command_router.h"

void process(const String &cmd);

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  initLed();

  initShiftRegister();
  setShiftRegister(0xFF);

  initCamera();

  commandRouterBegin(process);
  logMessage("INITCMD");

  //  initSoftAP();// Test
  //  startCameraServer();// Test
  //  blinkWifiLed();// Test

  //  randomLight(true);// Test

  clearWifiLed(true);
  clearShiftRegister(true);
  logMessage("SETUP");
}

void loop() {
  auto now = millis();
  auto isEnd = loopLed(now);
  if (isEnd && isAPOn()) {
    log_d("Recall Blink LED");
    blinkWifiLed();
  }
  commandRouterLoop(now);
  loopShiftRegister(now);
}

void process(const String &cmd) {
  bool isUnknown = false;
  if (cmd == CMD_WIFI_ON) {
    if (!isAPOn()) {
      initSoftAP();
      startCameraServer();
      blinkWifiLed();
    }
    sendCommand(isAPOn() ? CMD_WIFI_IS_ON : CMD_WIFI_IS_OFF);
  } else if (cmd == CMD_WIFI_OFF) {
    if (isAPOn()) {
      stopCameraServer();
      closeSoftAP();
      clearWifiLed(true);
    }
    sendCommand(isAPOn() ? CMD_WIFI_IS_ON : CMD_WIFI_IS_OFF);
  } else if (cmd == CMD_FOOL) {
    warningMessage();
  } else if (cmd == CMD_RANDOM1) {
    randomLight(true);
  } else if (cmd == CMD_RANDOM2) {
    randomLight(false);
  } else if (cmd == CMD_LIGHT_ON) {
    setShiftRegister(0xFF);
  } else if (cmd == CMD_LIGHT_OFF) {
    clearShiftRegister();
  } else {
    isUnknown = true;
    log_w("Unhandled command : %s", cmd);
  }

  if (isUnknown)
    sendCommand(CMD_UNKNOWN);
  else
    ackCommand(cmd);
}
