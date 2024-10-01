#include "command_router.h"
#include "esp32-hal-log.h"

#include "pins.h"

#define COMMAND_DELIMITER "/|"
#define COMMAND_DELIMITER_SIZE 2
#define MAX_COMMAND_BUFFER_SIZE 50

CommandCallback callback;

void commandRouterBegin(CommandCallback cb) {
  cmdSerial.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
  callback = cb;
  log_i("Commander setup");
}

unsigned long lastTime = 0;
unsigned long bufferSetUntil = 0;
String cmdBuffer = "";
void commandRouterLoop(unsigned long now) {
  if (now - lastTime > 1000 * 10) {
    lastTime = now;
    cmdSerial.printf("Keep from HEAD");
    cmdSerial.printf(COMMAND_DELIMITER);
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
      log_d("Buffer ::%s::", cmdBuffer);
    }
    // Check size of command-buffer
    if (cmdBuffer.length() > MAX_COMMAND_BUFFER_SIZE) {
      cmdBuffer = "";
      log_d("Clear Buffer!");
    } else {
      while (-1 != cmdBuffer.indexOf(COMMAND_DELIMITER)) {
        int found = cmdBuffer.indexOf(COMMAND_DELIMITER);
        if (found != -1) {
          String cmd = cmdBuffer.substring(0, found);
          cmdBuffer = cmdBuffer.substring(found + COMMAND_DELIMITER_SIZE);
          log_d("<=  %s ===", cmd);
          callback(cmd);
        }
      }
    }

    bufferSetUntil = now + 1000;
  }
}

void ackCommand(const String &cmd) {
  cmdSerial.printf("ACK.");
  cmdSerial.printf(COMMAND_DELIMITER);
  cmdSerial.flush();

  log_d("=> ACK : %s", cmd);
}

void sendCommand(const String &cmd) {
  cmdSerial.printf(cmd.c_str());
  cmdSerial.printf(COMMAND_DELIMITER);
  cmdSerial.flush();

  log_d("<= Send : %s", cmd);
}
