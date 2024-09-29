//
// Created by painnick on 2024-09-28.
//

#ifndef BODY_MCU_COMMAND_ROUTER_H
#define BODY_MCU_COMMAND_ROUTER_H

#include "SoftwareSerial.h"

#include "../command.h"

#define ROUTER_TAG "ROUTER"

class CommandRouter;

typedef std::function<void(const CommandRouter *, const String &cmd)>
    CommandCallback;

class CommandRouter {
 public:
  String cmdBuffer = "";

  explicit CommandRouter(SoftwareSerial &serial);
  ~CommandRouter();

  void init(CommandCallback callback);
  void begin(uint32_t baud, EspSoftwareSerial::Config config, int8_t rxPin, int8_t txPin);
  void loop();
  void send(const char *msg);

 protected:
  SoftwareSerial &serial;
  CommandCallback proc;
};

#endif// BODY_MCU_COMMAND_ROUTER_H
