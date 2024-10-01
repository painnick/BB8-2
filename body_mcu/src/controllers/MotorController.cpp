#include "MotorController.h"

#include <utility>

#define MOTOR_TAG "MOTOR"

MotorController::MotorController(uint8_t leftPin, uint8_t rightPin) : pin_left(leftPin), pin_right(rightPin), startMoveMs(0), endMoveMs(0), dir(MOTOR_DIRECTION::STOP) {}

MotorController::~MotorController() = default;

void MotorController::init() {
  pinMode(pin_left, OUTPUT);
  pinMode(pin_right, OUTPUT);
  internalStop();
}

void MotorController::left(unsigned long ms,
                           MotorCallback cb,
                           unsigned long startDelayMs) {
  callback = std::move(cb);
  unsigned long now = millis();
  endMoveMs = now + ms + startDelayMs;
  if (startDelayMs == 0) {
    startMoveMs = 0;
    internalLeft();
  } else {
    dir = MOTOR_DIRECTION::LEFT;
    startMoveMs = now + startDelayMs;
  }
}

void MotorController::right(unsigned long ms,
                            MotorCallback cb,
                            unsigned long startDelayMs) {
  callback = cb;
  unsigned long now = millis();
  endMoveMs = now + ms + startDelayMs;
  if (startDelayMs == 0) {
    startMoveMs = 0;
    internalRight();
  } else {
    dir = MOTOR_DIRECTION::RIGHT;
    startMoveMs = now + startDelayMs;
  }
}

void MotorController::stop(unsigned long ms,
                           MotorCallback cb,
                           unsigned long startDelayMs) {
  callback = cb;
  unsigned long now = millis();
  endMoveMs = now + ms + startDelayMs;
  if (startDelayMs == 0) {
    startMoveMs = 0;
    internalStop();
  } else {
    dir = MOTOR_DIRECTION::RIGHT;
    startMoveMs = now + startDelayMs;
  }
}

void MotorController::loop(unsigned long now) {
  if (startMoveMs != 0) {
    if (now > startMoveMs) {
      startMoveMs = 0;
      if (dir == MOTOR_DIRECTION::LEFT) {
        internalLeft();
      } else if (dir == MOTOR_DIRECTION::RIGHT) {
        internalRight();
      }
    }
  }

  if (dir != MOTOR_DIRECTION::STOP) {
    if (endMoveMs - now < 100) {
      MotorCallback lastCallback = callback;
      MOTOR_DIRECTION lastDir = dir;
      internalStop();
      if (lastCallback != nullptr) {
        lastCallback(this, lastDir);
      }
    }
  }
}

void MotorController::internalLeft() {
  dir = MOTOR_DIRECTION::LEFT;
  digitalWrite(pin_left, HIGH);
  digitalWrite(pin_right, LOW);

  ESP_LOGD(MOTOR_TAG, "Left");
}

void MotorController::internalRight() {
  dir = MOTOR_DIRECTION::RIGHT;
  digitalWrite(pin_left, LOW);
  digitalWrite(pin_right, HIGH);

  ESP_LOGD(MOTOR_TAG, "Right");
}

void MotorController::internalStop() {
  dir = MOTOR_DIRECTION::STOP;
  digitalWrite(pin_left, LOW);
  digitalWrite(pin_right, LOW);

  ESP_LOGD(MOTOR_TAG, "Stop");
}

void MotorController::randomMove(unsigned long duration,
                             const MotorCallback &cb,
                             unsigned long delay) {
  if ((random(1024) % 2) == 0)
    left(duration, cb, delay);
  else
    right(duration, cb, delay);
}

void MotorController::moveOpposite(
    unsigned long duration,
    const MotorCallback &cb) {
  if (dir == MOTOR_DIRECTION::LEFT || dir == MOTOR_DIRECTION::STOP) {
    stop(100, [this, duration, cb](MotorController *, MOTOR_DIRECTION) {
      right(duration, cb);
    });
  } else if (dir == MOTOR_DIRECTION::RIGHT) {
    stop(100, [this, duration, cb](MotorController *, MOTOR_DIRECTION) {
      left(duration, cb);
    });
  }
}
