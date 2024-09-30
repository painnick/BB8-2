#include "esp32-hal-log.h"
#include "esp_camera.h"
#include <Arduino.h>
#include <WiFi.h>

#include "app_httpd.h"

#define ARDUHAL_LOG_LEVEL ARDUHAL_LOG_LEVEL_WARN

#define PIN_RX 15
#define PIN_TX 14

#define cmdSerial Serial1
#define COMMAND_DELIMETER "/|"
#define COMMAND_DELIMETER_SIZE 2
#define MAX_COMMAND_BUFFER_SZIE 50

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
// #define CAMERA_MODEL_AI_THINKER // Has PSRAM
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

void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;// for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    log_e("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);      // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2);// lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }
  log_i("Camera setup");
}

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
