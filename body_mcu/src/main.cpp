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

VoiceCommander vc02(Serial1);
BluetoothController bt;
ShiftRegisterController shiftRegister(SR_DATA_PIN, SR_LATCH_PIN, SR_CLOCK_PIN);
SoftwareSerial cmdSerial;
HeadCommander head(cmdSerial);
MotorController motorController(MOTOR1_PIN, MOTOR2_PIN);
StateLedController stateLed(WIFI_EYE_PIN, WIFI_EYE_CH);

bool isListening = false;
bool moveHeadToFront = false;
bool isWifiOn = false;

void setup() {
  motorController.init();

  vc02.begin(115200, SERIAL_8N1, VOICE_COMMAND_RX_PIN, VOICE_COMMAND_TX_PIN);
  vc02.init([=](const VoiceCommander *cmd, const VoiceCommandType cmdType) {
    switch (cmdType) {
      case VC02_WAKE_UP:
        head.send(HEAD_WIFI_ON);
        isListening = true;
        stateLed.on();
        break;
      case VC02_SLEEP:
        head.send(HEAD_WIFI_OFF);
        isListening = false;
        stateLed.blink();
        break;
      case VC02_STOP:
        motorController.stop(0);
        stopMusic();
        // TODO : ...
        break;
      case VC02_TURN_LEFT:
        moveHeadToFront = false;
        motorController.left(1000);
        break;
      case VC02_TURN_RIGHT:
        moveHeadToFront = false;
        motorController.left(1000);
        break;
      case VC02_TURN_ON_LIGHT:
        head.send(HEAD_LIGHT_ON);
        break;
      case VC02_TURN_OFF_LIGHT:
        head.send(HEAD_LIGHT_OFF);
        break;
      case VC02_TURN_ON_AP:
        // TODO
        break;
      case VC02_TURN_OFF_AP:
        // TODO
        break;
      case VC02_TURN_ON_BLUETOOTH:
        // TODO
        break;
      case VC02_TURN_OFF_BLUETOOTH:
        // TODO
        break;
      case VC02_PLAY_MUSIC:
        playMusic();
        break;
      case VC02_FOOL:
        head.send(HEAD_WARN);
        // TODO : Motor control
        break;
      case VC02_LOOK_AT_ME:
        // TODO
        break;
      case VC02_ATTENTION:
        moveHeadToFront = true;
        motorController.randomMove(1000 * 10);
        break;
      case VC02_UNKNOWN:
      default:
        playFail();
        break;
    }
  });

  bt.begin("BB-8");
  bt.init([=](BluetoothController *controller, BluetoothCommandType cmdType, String &msg) {
    switch (cmdType) {
      case BT_HELP:
        controller->printHelp();
        break;
      case BT_ACK:
        // Do nothing
        break;
      case BT_WIFI_ON:
        // TODO
        break;
      case BT_WIFI_OFF:
        // TODO
        break;
      case BT_WARN:
        // TODO
        break;
      case BT_RANDOM_LIGHT1:
        // TODO
        break;
      case BT_RANDOM_LIGHT2:
        // TODO
        break;
      case BT_LIGHT_ON:
        // TODO
        break;
      case BT_LIGHT_OFF:
        // TODO
        break;
      case BT_TURN_LEFT:
        // TODO
        break;
      case BT_TURN_RIGHT:
        // TODO
        break;
      case BT_UNKNOWN:
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
      case HEAD_WARN:
        // TODO
        break;
      case HEAD_RANDOM_LIGHT1:
        // TODO
        break;
      case HEAD_RANDOM_LIGHT2:
        // TODO
        break;
      case HEAD_LIGHT_ON:
        // TODO
        break;
      case HEAD_LIGHT_OFF:
        // TODO
        break;
      case HEAD_TURN_LEFT:
        // TODO
        break;
      case HEAD_TURN_RIGHT:
        // TODO
        break;
      case HEAD_WIFI_IS_ON:
        isWifiOn = true;
        break;
      case HEAD_WIFI_IS_OFF:
        isWifiOn = false;
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

  shiftRegister.set(0xFF, 0xFF);

  head.send(HEAD_WIFI_OFF);
}

uint32_t lastAliveSoundChecked = 0;

void loop() {
  vc02.loop();
  bt.loop();

  auto now = millis();
  if (now - lastAliveSoundChecked > ALIVE_SOUND_INTERVAL_MS) {
    if (isListening)
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
  if (isEnd && !isListening) {
    stateLed.blink();
  }
}
