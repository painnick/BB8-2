#include "esp_log.h"
#include <Arduino.h>
#include <HardwareSerial.h>

#define DISABLE_MP3_DEBUG_LOG

#include "command.h"
#include "controllers/BluetoothController.h"
#include "controllers/CommandRouter.h"
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
CommandRouter router(cmdSerial);
MotorController motorController(MOTOR1_PIN, MOTOR2_PIN);
StateLedController stateLed(WIFI_EYE_PIN, WIFI_EYE_CH);

bool isListening = false;
bool moveHeadToFront = false;
bool isWifiOn = false;

void processCommand(Command cmd);

void setup() {
  motorController.init();

  vc02.begin(115200, SERIAL_8N1, VOICE_COMMAND_RX_PIN, VOICE_COMMAND_TX_PIN);
  vc02.init([=](const VoiceCommander *cmd, const VoiceCommandType cmdType) {
    switch (cmdType) {
      case VC02_WAKE_UP:
        router.send("WIFION");
        isListening = true;
        stateLed.on();
        break;
      case VC02_SLEEP:
        router.send("WIFIOFF");
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
        router.send("LIGHTON");
        break;
      case VC02_TURN_OFF_LIGHT:
        router.send("LIGHTOFF");
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
        router.send("WARN");
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

  router.begin(9600, SWSERIAL_8N1, HEAD_COMMAND_RX_PIN, HEAD_COMMAND_TX_PIN);
  router.init([=](const CommandRouter *router, const String &msg) {
    if (msg == "WIFIISON") {
      isWifiOn = true;
    } else if (msg == "WIFIISOFF") {
      isWifiOn = false;
    } else if ((msg == "ACK") || (msg == "InitCMD") || (msg == "SETUP") || (msg == "BufFull")) {
      // Skip
    } else {
      auto cmd = ToCommand(msg);
      if ((cmd & Command::NOT_COMMAND) != Command::NOT_COMMAND) {
        ESP_LOGD(MAIN_TAG, "Router Cmd : %s", ToString(cmd).c_str());
        processCommand(cmd);
      }
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

  router.send("WIFIOFF");
}

uint32_t lastAliveSoundChecked = 0;

void loop() {
  vc02.loop();

  Command cmd;
  cmd = bt.receive();
  if ((cmd & Command::NO_COMMAND) != Command::NO_COMMAND) {
    ESP_LOGD(MAIN_TAG, "BT Cmd : %s", ToString(cmd).c_str());
    if ((cmd & Command::NOT_COMMAND) != Command::NOT_COMMAND)
      processCommand(cmd);
  }

  auto now = millis();
  if (now - lastAliveSoundChecked > ALIVE_SOUND_INTERVAL_MS) {
    setDefaultVolume();
    playAlive();
    if (isListening)
      vc02.send(0xD3);
    lastAliveSoundChecked = now;
  }
  dfmp3.loop();
  shiftRegister.loop(now);
  router.loop();
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

void processCommand(Command cmd) {
  switch (cmd) {
    case Command::NOT_COMMAND:
    case Command::NO_COMMAND:
    case Command::UNKNOWN:
      // Do not update lastAliveSoundChecked
      break;
    default:lastAliveSoundChecked = millis();
      break;
  }
  switch (cmd) {
    case Command::WAKE_UP:router.send("WIFION");
      isListening = true;
      stateLed.on();
      break;
    case Command::BYE:router.send("WIFIOFF");
      isListening = false;
      stateLed.blink();
      break;
    case Command::TURN_LEFT:moveHeadToFront = false;
      motorController.left(1000);
      break;
    case Command::TURN_RIGHT:moveHeadToFront = false;
      motorController.right(1000);
      break;
    case Command::PLAY_MUSIC:playMusic();
      break;
    case Command::FOOL:router.send("WARN");
      // TODO : Motor control
      break;
    case Command::STOP:motorController.stop(0);
      stopMusic();
      // TODO : ...
      break;
    case Command::TURN_ON:router.send("LIGHTON");
      break;
    case Command::TURN_OFF:router.send("LIGHTOFF");
      break;
    case Command::UNKNOWN:playFail();
      break;
    case Command::WHERE_ARE_YOU:moveHeadToFront = true;
      motorController.randomMove(1000 * 10);
      break;
    default:break;
  }
}