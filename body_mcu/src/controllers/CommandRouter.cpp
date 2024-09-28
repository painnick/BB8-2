//
// Created by painnick on 2024-09-28.
//

#include "CommandRouter.h"

#define COMMAND_DELIMITER "/|"
#define COMMAND_DELIMITER_SIZE 2
#define MAX_COMMAND_BUFFER_SIZE 50

CommandRouter::CommandRouter(SoftwareSerial &serial) : serial(serial) {}
CommandRouter::~CommandRouter() {}

void CommandRouter::init(CommandCallback callback) {
  proc = std::move(callback);
}
void CommandRouter::begin(uint32_t baud, EspSoftwareSerial::Config config,
                          int8_t rxPin, int8_t txPin) {
  serial.begin(baud, config, rxPin, txPin);
}

void CommandRouter::loop() {
  if (serial.available()) {
    // Append command-buffer
    while (serial.available()) {
      cmdBuffer += (char)serial.read();
    }
    // Check size of command-buffer
    if (cmdBuffer.length() > MAX_COMMAND_BUFFER_SIZE) {
      cmdBuffer = "";
    } else {
      while (-1 != cmdBuffer.indexOf(COMMAND_DELIMITER)) {
        int found = cmdBuffer.indexOf(COMMAND_DELIMITER);
        if (found != -1) {
          String cmd = cmdBuffer.substring(0, found);
          cmdBuffer = cmdBuffer.substring(found + COMMAND_DELIMITER_SIZE);

          ESP_LOGV(ROUTER_TAG, "(HEAD) <= %s", cmd.c_str());

          proc(this, cmd);
        }
      }
    }
  }
}

void CommandRouter::send(const char *msg) {
  serial.printf("%s", msg);
  serial.printf(COMMAND_DELIMITER);
  serial.flush();

  ESP_LOGD(ROUTER_TAG, "(BODY) => %s", msg);
}