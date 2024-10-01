#ifndef BODY_MCU_SRC_CONTROLLERS_MOTORCONTROLLER_H_
#define BODY_MCU_SRC_CONTROLLERS_MOTORCONTROLLER_H_

#include <Arduino.h>
#include <functional>

#include "esp_log.h"

enum class MOTOR_DIRECTION {
  STOP = 0,
  LEFT = 1,
  RIGHT = 2,
};

class MotorController;

typedef std::function<void(MotorController *, MOTOR_DIRECTION)> MotorCallback;

class MotorController {
 public:
  MotorController(uint8_t leftPin, uint8_t rightPin);
  ~MotorController();

  void init();
  void left(unsigned long ms, MotorCallback callback = nullptr, unsigned long startDeltaMs = 0);
  void right(unsigned long ms, MotorCallback callback = nullptr, unsigned long startDeltaMs = 0);
  void stop(unsigned long ms, MotorCallback callback = nullptr, unsigned long startDeltaMs = 0);

  void loop(unsigned long now);

  void randomMove(unsigned long duration, const MotorCallback &callback = nullptr, unsigned long startDelayMs = 0);
  void moveOpposite(unsigned long duration, const MotorCallback &cb = nullptr);

 private:
  uint8_t pin_left;
  uint8_t pin_right;

  unsigned long startMoveMs;
  unsigned long endMoveMs;
  MOTOR_DIRECTION dir;
  MotorCallback callback;

  void internalLeft();
  void internalRight();
  void internalStop();
};

#endif//BODY_MCU_SRC_CONTROLLERS_MOTORCONTROLLER_H_
