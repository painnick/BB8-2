#include "esp_log.h"
#include <Arduino.h>
#include <HardwareSerial.h>

#define DISABLE_MP3_DEBUG_LOG

#include "controllers/BluetoothController.h"
#include "controllers/HeadCommander.h"
#include "controllers/MotorController.h"
#include "controllers/Mp3Controller.h"
#include "controllers/ShiftRegisterController.h"
#include "controllers/VoiceCommander.h"
#include "controllers/StateLedController.h"
#include "pinmap.h"

#define MAIN_TAG "Main"

#define ALIVE_SOUND_INTERVAL_MS 30000
#define BLUETOOTH_NAME "BB-8"

VoiceCommander vc02(Serial1);
BluetoothController bt(BLUETOOTH_NAME);
ShiftRegisterController shiftRegister(SR_DATA_PIN, SR_LATCH_PIN, SR_CLOCK_PIN);
SoftwareSerial cmdSerial;
HeadCommander head(cmdSerial);
MotorController motorController(MOTOR1_PIN, MOTOR2_PIN);
StateLedController stateLed(WIFI_EYE_PIN, WIFI_EYE_CH);

bool moveHeadToFront = false;

void DoWakeUp() {
  head.send(HEAD_WIFI_ON);
  stateLed.on();
  moveHeadToFront = true;
  motorController.randomMove(1000 * 10);
}

void DoSleep() {
  head.send(HEAD_WIFI_OFF);
  shiftRegister.clear();
  head.send(HEAD_LIGHT_OFF);
  stateLed.blink();
}

void DoStop() {
  motorController.stop(0);
  stopMusic();
}

void DoTurnHeadLeft() {
  moveHeadToFront = false;
  motorController.left(500);
}

void DoTurnHeadRight() {
  moveHeadToFront = false;
  motorController.right(500);
}

void DoTurnHeadLeft2() {
  moveHeadToFront = false;
  motorController.left(300);
}

void DoTurnHeadRight2() {
  moveHeadToFront = false;
  motorController.right(300);
}

void DoTurnOnLight() {
  head.send(HEAD_LIGHT_ON);
  shiftRegister.set(0xFF, 0xFF);
}

void DoTurnOffLight() {
  head.send(HEAD_LIGHT_OFF);
  shiftRegister.clear();
}

void DoWifiOn() {
  head.send(HEAD_WIFI_ON);
}

void DoWifiOff() {
  head.send(HEAD_WIFI_OFF);
}

void DoBluetoothOn() {
  bt.begin();
}

void DoBluetoothOff() {
  bt.close();
}

void DoFool() {
  head.send(HEAD_FOOL);
  shiftRegister.warningMessage();
  motorController.randomMove(500, [=](MotorController *c1, MOTOR_DIRECTION dir1) {
    motorController.moveOpposite(500, dir1, [=](MotorController *c2, MOTOR_DIRECTION dir2) {
      motorController.moveOpposite(300, dir2, [=](MotorController *c3, MOTOR_DIRECTION dir3) {
      });
    });
  }, 0);
}

void DoLookAtMe() {
  // TODO.
}

void DoAttention() {
  moveHeadToFront = true;
  motorController.randomMove(1000 * 10);
}

void setup() {
  motorController.init();

  vc02.begin(115200, SERIAL_8N1, VOICE_COMMAND_RX_PIN, VOICE_COMMAND_TX_PIN);
  vc02.init([=](const VoiceCommander *cmd, const VoiceCommandType cmdType) {
    switch (cmdType) {
      case VC02_WAKE_UP:
        DoWakeUp();
        break;
      case VC02_SLEEP:
        DoSleep();
        break;
      case VC02_STOP:
        DoStop();
        break;
      case VC02_TURN_LEFT:
        DoTurnHeadLeft();
        break;
      case VC02_TURN_RIGHT:
        DoTurnHeadRight();
        break;
      case VC02_TURN_ON_LIGHT:
        DoTurnOnLight();
        break;
      case VC02_TURN_OFF_LIGHT:
        DoTurnOffLight();
        break;
      case VC02_TURN_ON_AP:
        DoWifiOn();
        break;
      case VC02_TURN_OFF_AP:
        DoWifiOff();
        break;
      case VC02_TURN_ON_BLUETOOTH:
        DoBluetoothOn();
        break;
      case VC02_TURN_OFF_BLUETOOTH:
        DoBluetoothOff();
        break;
      case VC02_PLAY_MUSIC:
        playMusic();
        break;
      case VC02_FOOL:
        DoFool();
        break;
      case VC02_LOOK_AT_ME:
        DoLookAtMe();
        break;
      case VC02_ATTENTION:
        DoAttention();
        break;
      case VC02_UNKNOWN:
      default:
        playFail();
        break;
    }
  });

  bt.begin();
  bt.init([=](BluetoothController *controller, BluetoothCommandType cmdType, String &msg) {
    switch (cmdType) {
      case BT_HELP:
        controller->printHelp();
        break;
      case BT_ACK:
        // Do nothing
        break;
      case BT_WAKE_UP:
        DoWakeUp();
        break;
      case BT_SLEEP:
        DoSleep();
        break;
      case BT_STOP:
        DoStop();
        break;
      case BT_TURN_LEFT:
        DoTurnHeadLeft();
        break;
      case BT_TURN_RIGHT:
        DoTurnHeadRight();
        break;
      case BT_LIGHT_ON:
        DoTurnOnLight();
        break;
      case BT_LIGHT_OFF:
        DoTurnOffLight();
        break;
      case BT_WIFI_ON:
        DoWifiOn();
        break;
      case BT_WIFI_OFF:
        DoWifiOff();
        break;
      case BT_PLAY_MUSIC:
        playMusic();
        break;
      case BT_FOOL:
        DoFool();
        break;
      case BT_LOOK_AT_ME:
        DoLookAtMe();
        break;
      case BT_ATTENTION:
        DoAttention();
        break;
      case BT_UNKNOWN:
        bt.println("Type 'HELP'");
      default:
        playFail();
        break;
    }
  });

  head.begin(9600, SWSERIAL_8N1, HEAD_COMMAND_RX_PIN, HEAD_COMMAND_TX_PIN);
  head.init([=](const HeadCommander *router, HeadCommandType cmdType) {
    switch (cmdType) {
      case HEAD_ACK:
      case HEAD_LOG:
        // Do nothing
        break;
      case HEAD_WIFI_ON:
      case HEAD_WIFI_OFF:
        // Unavailable
        break;
      case HEAD_FOOL:
        DoFool();
        break;
      case HEAD_LIGHT_ON:
        DoTurnOnLight();
        break;
      case HEAD_LIGHT_OFF:
        DoTurnOffLight();
        break;
      case HEAD_TURN_LEFT:
        DoTurnHeadLeft();
        break;
      case HEAD_TURN_RIGHT:
        DoTurnHeadRight();
        break;
      case HEAD_TURN_LEFT2:
        DoTurnHeadLeft2();
        break;
      case HEAD_TURN_RIGHT2:
        DoTurnHeadRight2();
        break;
      case HEAD_WIFI_IS_ON:
        break;
      case HEAD_WIFI_IS_OFF:
        break;
      case HEAD_UNKNOWN:
      default:
        playFail();
        break;
    }
  });

  setupSound();
  setDefaultVolume();
  delay(1000 * 3);
  playWelcome();

  auto hallVal = analogRead(HALL_SENSOR_PIN);
  if (hallVal != 0) {
    moveHeadToFront = true;
    motorController.randomMove(5000);
  }

  shiftRegister.set(0x00, 0x00);

  head.send(HEAD_WIFI_OFF);
}

uint32_t lastAliveSoundChecked = 0;

void loop() {
  vc02.loop();
  bt.loop();

  auto now = millis();
  if (now - lastAliveSoundChecked > ALIVE_SOUND_INTERVAL_MS) {
    if (vc02.isListening)
      vc02.send(VC02_KEEPALIVE);
    lastAliveSoundChecked = now;
  }
  dfmp3.loop();
  shiftRegister.loop(now);
  head.loop();
  motorController.loop(now);

  if (moveHeadToFront) {
    auto hallVal = analogRead(HALL_SENSOR_PIN);
    if (hallVal == 0) {
      moveHeadToFront = false;
      motorController.stop(1000);
    }
  }

  auto isEnd = stateLed.loop(now);
  if (isEnd && !vc02.isListening) {
    stateLed.blink();
  }
}
