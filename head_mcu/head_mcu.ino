#include "esp32-hal-log.h"
#include <Arduino.h>

#include "ap_control.h"
#include "app_httpd.h"
#include "camera_control.h"
#include "pins.h"
#include "command_router.h"

#define ARDUHAL_LOG_LEVEL ARDUHAL_LOG_LEVEL_WARN

void process(const String &cmd);

bool isWifiOn = true;

void setup() {
  initCamera();

  commandRouterBegin(process);

  if (isWifiOn) {
    initSoftAP();
    startCameraServer();
    log_i("Camera Ready! Use 'http://%s' to connect", WiFi.softAPIP());
  }
}

void loop() {
  commandRouterLoop();
}

void process(const String &cmd) {
  if (cmd == "WIFION") {
    if (!isWifiOn) {
      initSoftAP();
      startCameraServer();
      isWifiOn = true;
    }
    ackCommand(cmd);
  } else if (cmd == "WIFIOFF") {
    if (isWifiOn) {
      stopCameraServer();
      closeSoftAP();
      isWifiOn = false;
    }
    ackCommand(cmd);
    //  } else if (cmd == "LED1ON") {
    //    digitalWrite(PIN_LED1, HIGH);
    //    ackCommand(cmd);
    //  } else if (cmd == "LED1OFF") {
    //    digitalWrite(PIN_LED1, LOW);
    //    ackCommand(cmd);
    //  } else if (cmd == "LED2ON") {
    //    digitalWrite(PIN_LED2, HIGH);
    //    ackCommand(cmd);
    //  } else if (cmd == "LED2OFF") {
    //    digitalWrite(PIN_LED2, LOW);
    //    ackCommand(cmd);
    //  } else if (cmd == "LED3ON") {
    //    digitalWrite(PIN_LED3, HIGH);
    //    ackCommand(cmd);
    //  } else if (cmd == "LED3OFF") {
    //    digitalWrite(PIN_LED3, LOW);
    //    ackCommand(cmd);
  } else {
    ackCommand("Unknown");
    log_w("Unhandled command : %s", cmd);
  }
}
