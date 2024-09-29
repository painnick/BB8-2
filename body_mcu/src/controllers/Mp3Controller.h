#pragma once

#include <Arduino.h>
#include <esp_log.h>

#include "../pinmap.h"
#include "DFMiniMp3.h"

#define MP3_TAG "DFPLAYER"

#ifdef DEBUG
#define DEFAULT_VOLUME 10
#else
#define DEFAULT_VOLUME 15
#endif

int BACKGROUND_TRACK = -1;

class Mp3Notify;

typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3;

class Mp3Notify {
 public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char *action) {
    if (source & DfMp3_PlaySources_Sd) {
      ESP_LOGD(MP3_TAG, "SD Card, %s", action);
    }
    if (source & DfMp3_PlaySources_Usb) {
      ESP_LOGD(MP3_TAG, "USB Disk, %s", action);
    }
    if (source & DfMp3_PlaySources_Flash) {
      ESP_LOGD(MP3_TAG, "Flash, %s", action);
    }
  }

  static void OnError(DfMp3 &mp3, uint16_t errorCode) {
    // see DfMp3_Error for code meaning
    switch (errorCode) {
      case DfMp3_Error_Busy:
        ESP_LOGE(MP3_TAG, "Com Error - Busy");
        break;
      case DfMp3_Error_Sleeping:
        ESP_LOGE(MP3_TAG, "Com Error - Sleeping");
        break;
      case DfMp3_Error_SerialWrongStack:
        ESP_LOGE(MP3_TAG, "Com Error - Serial Wrong Stack");
        break;

      case DfMp3_Error_RxTimeout:
        ESP_LOGE(MP3_TAG, "Com Error - Rx Timeout!!!");
        break;
      case DfMp3_Error_PacketSize:
        ESP_LOGE(MP3_TAG, "Com Error - Wrong Packet Size!!!");
        break;
      case DfMp3_Error_PacketHeader:
        ESP_LOGE(MP3_TAG, "Com Error - Wrong Packet Header!!!");
        break;
      case DfMp3_Error_PacketChecksum:
        ESP_LOGE(MP3_TAG, "Com Error - Wrong Packet Checksum!!!");
        break;

      default:
        ESP_LOGE(MP3_TAG, "Com Error - %d", errorCode);
        break;
    }
  }

  static void OnPlayFinished(DfMp3 &mp3, DfMp3_PlaySources source, const uint16_t track) {
    ESP_LOGD(MP3_TAG, "Play finished for #%d", track);
    if (track == BACKGROUND_TRACK) {
      //            playBackground();
    }
  }

  static void OnPlaySourceOnline(DfMp3 &mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "online");
  }

  static void OnPlaySourceInserted(DfMp3 &mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "inserted");
  }

  static void OnPlaySourceRemoved(DfMp3 &mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "removed");
  }
};

DfMp3 dfmp3(Serial2, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);

inline void setDefaultVolume() {
  ESP_LOGD(MP3_TAG, "Set Volume");
  dfmp3.setVolume(DEFAULT_VOLUME);
}

inline void setupSound() {
  ESP_LOGI(MP3_TAG, "Setup DFPlayer");
  dfmp3.begin(9600, 1000);
}

void playWelcome() {
  ESP_LOGD(MP3_TAG, "Play Welcome");
  dfmp3.playMp3FolderTrack(1);
}

void playHello() {
  ESP_LOGD(MP3_TAG, "Play Hello");
  dfmp3.playMp3FolderTrack(2);
}

void playFail() {
  ESP_LOGD(MP3_TAG, "Play Fail");
  dfmp3.playMp3FolderTrack(3);
}

void playWhy() {
  ESP_LOGD(MP3_TAG, "Play Why");
  dfmp3.playMp3FolderTrack(4);
}

void playBye() {
  ESP_LOGD(MP3_TAG, "Play Bye");
  dfmp3.playMp3FolderTrack(5);
}

void playAlive() {
  int num = random(6, 9);
  ESP_LOGD(MP3_TAG, "Play Alive #%d", num);
  dfmp3.playMp3FolderTrack(num);
}