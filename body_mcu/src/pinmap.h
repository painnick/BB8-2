#pragma once

// These are all GPIO pins on the ESP32
// Recommended pins include 2,4,12-19,21-23,25-27,32-33
// for the ESP32-S2 the GPIO pins are 1-21,26,33-42

#define MAIN_EYE_PIN 13

#define VOICE_COMMAND_TX_PIN 16
#define VOICE_COMMAND_RX_PIN 17

#define HEAD_COMMAND_TX_PIN 18
#define HEAD_COMMAND_RX_PIN 19

#define DFPLAYER_TX_PIN 32
#define DFPLAYER_RX_PIN 33

#define SR_DATA_PIN 25
#define SR_LATCH_PIN 26
#define SR_CLOCK_PIN 27
