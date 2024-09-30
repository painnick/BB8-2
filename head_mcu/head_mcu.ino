#include "esp32-hal-log.h"
#include <Arduino.h>

#include "ap_control.h"
#include "app_httpd.h"
#include "camera_control.h"
#include "pins.h"
#include "command_router.h"

#define ARDUHAL_LOG_LEVEL ARDUHAL_LOG_LEVEL_WARN

#define WIFI_LED_BRIGHT 17
#define WIFI_LED_DURATION_MS 200
#define WIFI_LED_INDEX_SIZE (1000 * 5 / WIFI_LED_DURATION_MS)

void process(const String &cmd);

bool isWifiOn = true;
int WifiLedIndex = WIFI_LED_INDEX_SIZE / 2;

void setup() {
  ledcAttachChannel(WIFI_LED_PIN, 1000, 8, WIFI_LED_CH);

  initCamera();

  commandRouterBegin(process);

  if (isWifiOn) {
    initSoftAP();
    startCameraServer();
    ledcWrite(WIFI_LED_PIN, WIFI_LED_BRIGHT);
    log_i("Camera Ready! Use 'http://%s' to connect", WiFi.softAPIP());
  }
}

unsigned long lastWifiLedChecked = 0;
void loop() {
  auto now = millis();
  if (isWifiOn) {
    if (now - lastWifiLedChecked > WIFI_LED_DURATION_MS) {
      WifiLedIndex = (WifiLedIndex + 1) % WIFI_LED_INDEX_SIZE;
      ledcWrite(WIFI_LED_PIN, WifiLedIndex == 0 ? WIFI_LED_BRIGHT : 0);
      lastWifiLedChecked = now;
    }
  }
  commandRouterLoop();
}



void process(const String &cmd) {
  if (cmd == "WIFION") {
    if (!isWifiOn) {
      initSoftAP();
      startCameraServer();
      ledcWrite(WIFI_LED_PIN, WIFI_LED_BRIGHT);
      isWifiOn = true;
    }
    ackCommand(cmd);
  } else if (cmd == "WIFIOFF") {
    if (isWifiOn) {
      stopCameraServer();
      closeSoftAP();
      ledcWrite(WIFI_LED_PIN, 0);
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
