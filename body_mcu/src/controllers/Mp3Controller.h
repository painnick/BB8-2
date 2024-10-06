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
#ifndef DISABLE_MP3_DEBUG_LOG
      ESP_LOGD(MP3_TAG, "SD Card, %s", action);
#endif
    }
    if (source & DfMp3_PlaySources_Usb) {
#ifndef DISABLE_MP3_DEBUG_LOG
      ESP_LOGD(MP3_TAG, "USB Disk, %s", action);
#endif
    }
    if (source & DfMp3_PlaySources_Flash) {
#ifndef DISABLE_MP3_DEBUG_LOG
      ESP_LOGD(MP3_TAG, "Flash, %s", action);
#endif
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
#ifndef DISABLE_MP3_DEBUG_LOG
    ESP_LOGD(MP3_TAG, "Play finished for #%d", track);
#endif
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
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Set Volume");
#endif
  dfmp3.setVolume(DEFAULT_VOLUME);
}

inline void setVolume(int vol) {
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Set Volume %d", vol);
#endif
  dfmp3.setVolume(vol);
}

inline void setupSound() {
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGI(MP3_TAG, "Setup DFPlayer");
#endif
  dfmp3.begin(9600, 1000);
}

void playWelcome() {
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Play Welcome");
#endif
  dfmp3.playMp3FolderTrack(1);
}

void playHello() {
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Play Hello");
#endif
  dfmp3.playMp3FolderTrack(2);
}

void playFail() {
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Play Fail");
#endif
  dfmp3.playMp3FolderTrack(3);
}

void playWhy() {
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Play Why");
#endif
  dfmp3.playMp3FolderTrack(4);
}

void playBye() {
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Play Bye");
#endif
  dfmp3.playMp3FolderTrack(5);
}

void playAlive() {
  int num = random(6, 9);
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Play Alive #%d", num);
#endif
  dfmp3.playMp3FolderTrack(num);
}

void playMusic() {
  int num = random(4) + 1;
#ifndef DISABLE_MP3_DEBUG_LOG
  ESP_LOGD(MP3_TAG, "Play music 01 folder #%d", num);
#endif
  dfmp3.playFolderTrack16(1, num);
}

void stopMusic() {
  dfmp3.stop();
}
