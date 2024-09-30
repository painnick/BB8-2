#include "esp32-hal-log.h"
#include <Arduino.h>
#include <WiFi.h>

#include "app_httpd.h"
#include "camera_controll.h"

#define ARDUHAL_LOG_LEVEL ARDUHAL_LOG_LEVEL_WARN

#define cmdSerial Serial1
#define COMMAND_DELIMETER "/|"
#define COMMAND_DELIMETER_SIZE 2
#define MAX_COMMAND_BUFFER_SZIE 50

#include "pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "BB-8";
const char *password = "aaaa1111";

#define SOFT_AP_SSID "BB-8"
#define SOFT_AP_PASSWORD "aaaa1111"

#define SOFT_AP_IP_ADDRESS_1 192
#define SOFT_AP_IP_ADDRESS_2 168
#define SOFT_AP_IP_ADDRESS_3 1
#define SOFT_AP_IP_ADDRESS_4 100

#define SOFT_AP_GW_ADDRESS_1 192
#define SOFT_AP_GW_ADDRESS_2 168
#define SOFT_AP_GW_ADDRESS_3 1
#define SOFT_AP_GW_ADDRESS_4 10

#define SOFT_AP_NM_ADDRESS_1 255
#define SOFT_AP_NM_ADDRESS_2 255
#define SOFT_AP_NM_ADDRESS_3 255
#define SOFT_AP_NM_ADDRESS_4 0

void initSoftAP() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  IPAddress local_ip(SOFT_AP_IP_ADDRESS_1, SOFT_AP_IP_ADDRESS_2, SOFT_AP_IP_ADDRESS_3, SOFT_AP_IP_ADDRESS_4);
  IPAddress gateway(SOFT_AP_GW_ADDRESS_1, SOFT_AP_GW_ADDRESS_2, SOFT_AP_GW_ADDRESS_3, SOFT_AP_GW_ADDRESS_4);
  IPAddress subnet(SOFT_AP_NM_ADDRESS_1, SOFT_AP_NM_ADDRESS_2, SOFT_AP_NM_ADDRESS_3, SOFT_AP_NM_ADDRESS_4);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(SOFT_AP_SSID /*, SOFT_AP_PASSWORD */);
  WiFi.setSleep(false);

  log_i("SoftAP setup");
}

void closeSoftAP() {
  WiFi.softAPdisconnect(true);
}

bool isWifiOn = true;

void setup() {
  cmdSerial.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);
  log_i("Commander setup");

  initCamera();

  if (isWifiOn) {
    initSoftAP();
    startCameraServer();
    log_i("Camera Ready! Use 'http://%s' to connect", WiFi.softAPIP());
  }
}

unsigned long lastTime = 0;
unsigned long bufferSetUntil = 0;
String cmdBuffer = "";
void loop() {
 unsigned long now = millis();
 if (now - lastTime > 1000 * 10) {
   lastTime = now;
   cmdSerial.printf("Keep from HEAD");
   cmdSerial.printf(COMMAND_DELIMETER);
   cmdSerial.flush();
   log_d("=> Keep from HEAD");
 }

 if (cmdSerial.available()) {
   if (now > bufferSetUntil) {
     cmdBuffer = "";
     log_w("Buffer Timeout");
   }

   // Append command-buffer
   while (cmdSerial.available()) {
     cmdBuffer += (char) cmdSerial.read();
     log_d("Buffer ::" + cmdBuffer + "::");
   }
   // Check size of command-buffer
   if (cmdBuffer.length() > MAX_COMMAND_BUFFER_SZIE) {
     cmdBuffer = "";
     log_d("Clear Buffer!");
   } else {
     while (-1 != cmdBuffer.indexOf(COMMAND_DELIMETER)) {
       int found = cmdBuffer.indexOf(COMMAND_DELIMETER);
       if (found != -1) {
         String cmd = cmdBuffer.substring(0, found);
         cmdBuffer = cmdBuffer.substring(found + COMMAND_DELIMETER_SIZE);
         log_d("<=  %s ===", cmd);
         process(cmd);
       }
     }
   }

   bufferSetUntil = now + 1000;
 }
}

void ackCommand(const String &cmd) {
  cmdSerial.printf("ACK.");
  cmdSerial.printf(COMMAND_DELIMETER);
  cmdSerial.flush();

  log_d("=> ACK : %s", cmd);
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
