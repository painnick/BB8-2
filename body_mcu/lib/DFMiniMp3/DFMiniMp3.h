/*-------------------------------------------------------------------------
DFMiniMp3 library

Written by Michael C. Miller.

I invest time and resources providing this open source code,
please support me by dontating (see https://github.com/Makuna/DFMiniMp3)

-------------------------------------------------------------------------
This file is part of the Makuna/DFMiniMp3 library.

DFMiniMp3 is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

DFMiniMp3 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with DFMiniMp3.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/
#pragma once

#define USE_MH2024K16SS

#include <Arduino.h>
#include <cstdint>
#include <esp_log.h>

#define DFPLAYER_TAG "DFMINI"

enum DfMp3_Error {
  // from device
  DfMp3_Error_Busy = 1,
  DfMp3_Error_Sleeping,
  DfMp3_Error_SerialWrongStack,
  DfMp3_Error_CheckSumNotMatch,
  DfMp3_Error_FileIndexOut,
  DfMp3_Error_FileMismatch,
  DfMp3_Error_Advertise,
  // from library
  DfMp3_Error_RxTimeout = 0x81,
  DfMp3_Error_PacketSize,
  DfMp3_Error_PacketHeader,
  DfMp3_Error_PacketChecksum,
  DfMp3_Error_General = 0xff
};

enum DfMp3_PlaybackMode {
  DfMp3_PlaybackMode_Repeat,
  DfMp3_PlaybackMode_FolderRepeat,
  DfMp3_PlaybackMode_SingleRepeat,
  DfMp3_PlaybackMode_Random
};

enum DfMp3_Eq {
  DfMp3_Eq_Normal,
  DfMp3_Eq_Pop,
  DfMp3_Eq_Rock,
  DfMp3_Eq_Jazz,
  DfMp3_Eq_Classic,
  DfMp3_Eq_Bass
};

enum DfMp3_PlaySource// value - only one can be set
{
  DfMp3_PlaySource_Usb = 1,
  DfMp3_PlaySource_Sd,
  DfMp3_PlaySource_Aux,
  DfMp3_PlaySource_Sleep,
  DfMp3_PlaySource_Flash
};

enum DfMp3_PlaySources// bitfield - more than one can be set
{
  DfMp3_PlaySources_Usb = 0x01,
  DfMp3_PlaySources_Sd = 0x02,
  DfMp3_PlaySources_Pc = 0x04,
  DfMp3_PlaySources_Flash = 0x08,
};

// 7E FF 06 0F 00 01 01 xx xx EF
// 0	->	7E is start code
// 1	->	FF is version
// 2	->	06 is length
// 3	->	0F is command
// 4	->	00 is no receive
// 5~6	->	01 01 is argument
// 7~8	->	checksum = 0 - ( FF+06+0F+00+01+01 )
// 9	->	EF is end code
struct DfMp3_Packet_WithCheckSum {
  uint8_t startCode;
  uint8_t version;
  uint8_t length;
  uint8_t command;
  uint8_t requestAck;
  uint8_t hiByteArgument;
  uint8_t lowByteArgument;
  uint8_t hiByteCheckSum;
  uint8_t lowByteCheckSum;
  uint8_t endCode;
};
struct DfMp3_Packet_WithoutCheckSum {
  uint8_t startCode;
  uint8_t version;
  uint8_t length;
  uint8_t command;
  uint8_t requestAck;
  uint8_t hiByteArgument;
  uint8_t lowByteArgument;
  uint8_t endCode;
};

uint16_t calcChecksum(const DfMp3_Packet_WithCheckSum &packet) {
  uint16_t sum = 0xFFFF;
  for (const uint8_t *packetByte = &(packet.version);
       packetByte != &(packet.hiByteCheckSum); packetByte++) {
    sum -= *packetByte;
  }
  return sum + 1;
}

void setChecksum(DfMp3_Packet_WithCheckSum *out) {
  uint16_t sum = calcChecksum(*out);

  out->hiByteCheckSum = (sum >> 8);
  out->lowByteCheckSum = (sum & 0xff);
}

bool validateChecksum(const DfMp3_Packet_WithCheckSum &in) {
  uint16_t sum = calcChecksum(in);
  return (sum == static_cast<uint16_t>((in.hiByteCheckSum << 8) | in.lowByteCheckSum));
}

class Mp3ChipMH2024K16SS {
 public:
  static const bool SendCheckSum = false;

  typedef DfMp3_Packet_WithoutCheckSum SendPacket;
  typedef DfMp3_Packet_WithCheckSum ReceptionPacket;

  static SendPacket generatePacket(uint8_t command, uint16_t arg) {
    return {0x7E,
            0xFF,
            6,
            command,
            0,
            static_cast<uint8_t>(arg >> 8),
            static_cast<uint8_t>(arg & 0x00ff),
            0xEF};
  }
};

class Mp3ChipOriginal {
 public:
  static const bool SendCheckSum = true;

  typedef DfMp3_Packet_WithCheckSum SendPacket;
  typedef DfMp3_Packet_WithCheckSum ReceptionPacket;

  static SendPacket generatePacket(uint8_t command, uint16_t arg) {
    SendPacket packet = {0x7E,
                         0xFF,
                         6,
                         command,
                         0,
                         static_cast<uint8_t>(arg >> 8),
                         static_cast<uint8_t>(arg & 0x00ff),
                         0,
                         0,
                         0xEF};
    setChecksum(&packet);
    return packet;
  }
};

#ifdef USE_MH2024K16SS

template<class T_SERIAL_METHOD,
         class T_NOTIFICATION_METHOD,
         class T_CHIP_VARIANT = Mp3ChipMH2024K16SS>
#else
template<class T_SERIAL_METHOD,
         class T_NOTIFICATION_METHOD,
         class T_CHIP_VARIANT = Mp3ChipOriginal>
#endif
class DFMiniMp3 {
 public:
  explicit DFMiniMp3(T_SERIAL_METHOD &serial, int8_t RX_Pin, int8_t TX_Pin)
      : _serial(serial), _lastSendSpace(c_msSendSpace), _isOnline(false),
        _rxPin(RX_Pin), _txPin(TX_Pin) {}

  void begin(unsigned long baud = 9600, unsigned long timeout = 10000) {
    if (_rxPin == -1 && _txPin == -1) {
      ESP_LOGE(DFPLAYER_TAG, "RX_Pin %d, TX_Pin %d", _rxPin, _txPin);
      return;
    }
    _serial.begin(baud, SERIAL_8N1, _rxPin, _txPin);
    _serial.setTimeout(timeout);
    delay(1000);
    _lastSend = millis();
  }

  void loop() {
    while (_serial.available() >= static_cast<int>(sizeof(typename T_CHIP_VARIANT::ReceptionPacket))) {
      listenForReply(0x00);
    }
  }

  // Does not work with all models.
  // YX5200-24SS - sends reply
  // MH2024K-24SS - sends NO reply --> results in error notification
  DfMp3_PlaySources getPlaySources() {
    drainResponses();
    return static_cast<DfMp3_PlaySources>(sendAndListenForReply(0x3f));
  }

  // the track as enumerated across all folders
  void playGlobalTrack(uint16_t track = 0) { sendPacket(0x03, track); }

  // sd:/mp3/####track name
  void playMp3FolderTrack(uint16_t track) { sendPacket(0x12, track); }

  // older devices: sd:/###/###track name
  // newer devices: sd:/##/###track name
  // folder and track numbers are zero padded
  void playFolderTrack(uint8_t folder, uint8_t track) {
    uint16_t arg = (folder << 8) | track;
    sendPacket(0x0f, arg);
  }

  // sd:/##/####track name
  // track number must be four digits, zero padded
  void playFolderTrack16(uint8_t folder, uint16_t track) {
    uint16_t arg = (((uint16_t) folder) << 12) | track;
    sendPacket(0x14, arg);
  }

  void playRandomTrackFromAll() { sendPacket(0x18); }

  void nextTrack() {
    sendPacket(0x01);
    listenForReply(0x01);
  }

  void prevTrack() { sendPacket(0x02); }

  uint16_t getCurrentTrack(DfMp3_PlaySource source = DfMp3_PlaySource_Sd) {
    drainResponses();

    uint8_t command;

    switch (source) {
      case DfMp3_PlaySource_Usb:
        command = 0x4b;
        break;
      case DfMp3_PlaySource_Sd:
        command = 0x4c;
        break;
      case DfMp3_PlaySource_Flash:
        command = 0x4d;
        break;
      default:
        command = 0x4c;
        break;
    }

    return sendAndListenForReply(command);
  }

  // 0- 30
  void setVolume(uint8_t volume) { sendPacket(0x06, volume); }

  uint8_t getVolume() {
    drainResponses();
    return sendAndListenForReply(0x43);
  }

  void increaseVolume() { sendPacket(0x04); }

  void decreaseVolume() { sendPacket(0x05); }

  // useless, removed
  // 0-31
  /*
  void setVolume(bool mute, uint8_t volume)
  {
      uint16_t arg = (!mute << 8) | volume;
      sendPacket(0x10, arg);
  }
  */

  void loopGlobalTrack(uint16_t globalTrack) { sendPacket(0x08, globalTrack); }

  // sd:/##/*
  // 0-99
  void loopFolder(uint8_t folder) { sendPacket(0x17, folder); }

  DfMp3_PlaybackMode getPlaybackMode() {
    drainResponses();
    return static_cast<DfMp3_PlaybackMode>(sendAndListenForReply(0x45));
  }

  void setRepeatPlayAllInRoot(bool repeat) { sendPacket(0x11, repeat); }

  void setRepeatPlayCurrentTrack(bool repeat) { sendPacket(0x19, !repeat); }

  void setEq(DfMp3_Eq eq) { sendPacket(0x07, eq); }

  DfMp3_Eq getEq() {
    drainResponses();
    return static_cast<DfMp3_Eq>(sendAndListenForReply(0x44));
  }

  void setPlaybackSource(DfMp3_PlaySource source) {
    sendPacket(0x09, source, 200);
  }

  void sleep() { sendPacket(0x0a); }

  void reset() {
    sendPacket(0x0c, 0, 600);
    _isOnline = false;
    listenForReply(0x00);
  }

  void start() { sendPacket(0x0d); }

  void pause() { sendPacket(0x0e); }

  void stop() { sendPacket(0x16); }

  uint16_t getStatus() {
    drainResponses();
    return sendAndListenForReply(0x42);
  }

  uint16_t getFolderTrackCount(uint16_t folder) {
    drainResponses();
    return sendAndListenForReply(0x4e, folder);
  }

  uint16_t getTotalTrackCount(DfMp3_PlaySource source) {
    drainResponses();

    uint8_t command;

    switch (source) {
      case DfMp3_PlaySource_Usb:
        command = 0x47;
        break;
      case DfMp3_PlaySource_Sd:
        command = 0x48;
        break;
      case DfMp3_PlaySource_Flash:
        command = 0x49;
        break;
      default:
        command = 0x48;
        break;
    }

    return sendAndListenForReply(command);
  }

  uint16_t getTotalFolderCount() {
    drainResponses();
    return sendAndListenForReply(0x4F);
  }

  // sd:/advert/####track name
  void playAdvertisement(uint16_t track) { sendPacket(0x13, track); }

  void stopAdvertisement() { sendPacket(0x15); }

  void enableDac() { sendPacket(0x1A, 0x00); }

  void disableDac() { sendPacket(0x1A, 0x01); }

  bool isOnline() const { return _isOnline; }

  void delayForResponse(int ms) {
    int cnt = ms / 10;
    for (int i = 0; i < cnt; i++) {
      loop();
      delay(10);
    }
  }

 private:
  static const uint16_t c_msSendSpace = 50;

  T_SERIAL_METHOD &_serial;
  int8_t _rxPin{-1};
  int8_t _txPin{-1};
  uint32_t _lastSend{};// not initialized as agreed in issue #63
  uint16_t _lastSendSpace;
  bool _isOnline;

  void drainResponses() {
    while (_serial.available() > 0) {
      listenForReply(0x00);
    }
  }

  void sendPacket(uint8_t command,
                  uint16_t arg = 0,
                  uint16_t sendSpaceNeeded = c_msSendSpace) {
    typename T_CHIP_VARIANT::SendPacket packet = T_CHIP_VARIANT::generatePacket(command, arg);

    // wait for spacing since last send
    while (((millis() - _lastSend) < _lastSendSpace)) {
      // check for event messages from the device while
      // we wait
      loop();
      delay(1);
    }

    _lastSendSpace = sendSpaceNeeded;

#ifndef DISABLE_MP3_DEBUG_LOG
#ifdef USE_MH2024K16SS
    ESP_LOGV(DFPLAYER_TAG,
             "==> STR:%02X VER:%02X LEN:%02X CMD:%02X ACK:%02X PAR1:%02X PAR2:%02X END:%02X",
             packet.startCode,
             packet.version,
             packet.length,
             packet.command,
             packet.requestAck,
             packet.hiByteArgument,
             packet.lowByteArgument,
             packet.endCode);
#else
    ESP_LOGV(DFPLAYER_TAG,
             "==> STR:%02X VER:%02X LEN:%02X CMD:%02X ACK:%02X PAR1:%02X PAR2:%02X CHK1:%02X CHK2:%02X",
             packet.startCode,
             packet.version,
             packet.length,
             packet.command,
             packet.requestAck,
             packet.hiByteArgument,
             packet.lowByteArgument,
             packet.hiByteCheckSum,
             packet.lowByteCheckSum,
             packet.endCode);
#endif
#endif

    _serial.write(reinterpret_cast<uint8_t *>(&packet), sizeof(packet));
    _serial.flush();

    _lastSend = millis();
  }

  bool readPacket(uint8_t *command, uint16_t *argument) {
    typename T_CHIP_VARIANT::ReceptionPacket in;
    uint8_t read;

    // init our out args always
    *command = 0;
    *argument = 0;

    // try to sync our reads to the packet start
    do {
      // we use readBytes as it gives us the standard timeout
      read = _serial.readBytes(&in.startCode, 1);

      if (read != 1) {
        // nothing read
        *argument = DfMp3_Error_RxTimeout;

        return false;
      }
    } while (in.startCode != 0x7e);

    read += _serial.readBytes(&in.version, sizeof(in) - 1);
    if (read < sizeof(in)) {
      // not enough bytes, corrupted packet
      *argument = DfMp3_Error_PacketSize;
      return false;
    }

    if (in.version != 0xFF || in.length != 0x06 || in.endCode != 0xef) {
      // invalid version or corrupted packet
      *argument = DfMp3_Error_PacketHeader;
      return false;
    }

    if (!validateChecksum(in)) {
      // checksum failed, corrupted packet
      *argument = DfMp3_Error_PacketChecksum;
      return false;
    }

#ifndef DISABLE_MP3_DEBUG_LOG
    ESP_LOGV(DFPLAYER_TAG, "<== CMD:%02X PAR1:%02X PAR2:%02X", in.command, in.hiByteArgument, in.lowByteArgument);
#endif
    *command = in.command;
    *argument = ((in.hiByteArgument << 8) | in.lowByteArgument);

    return true;
  }

  int32_t listenForReply(uint8_t command) {
    uint8_t replyCommand = 0;
    uint16_t replyArg = 0;

    do {
      bool read = readPacket(&replyCommand, &replyArg);
      if (read) {
        if (command != 0 && command == replyCommand) {
          return replyArg;
        } else {
#ifndef DISABLE_MP3_DEBUG_LOG
          ESP_LOGV(DFPLAYER_TAG, "replyCommand %02X", replyCommand);
#endif
          switch (replyCommand) {
            case 0x3c:// usb
            case 0x4b:// usb on MH2024K-16SS
              T_NOTIFICATION_METHOD::OnPlayFinished(*this, DfMp3_PlaySources_Usb, replyArg);
              break;

            case 0x3d:// micro sd
            case 0x4c:// micro sd on MH2024K-16SS
              T_NOTIFICATION_METHOD::OnPlayFinished(*this, DfMp3_PlaySources_Sd, replyArg);
              break;

            case 0x3e:// flash
              T_NOTIFICATION_METHOD::OnPlayFinished(
                  *this, DfMp3_PlaySources_Flash, replyArg);
              break;

            case 0x3F:
              _isOnline = true;
              T_NOTIFICATION_METHOD::OnPlaySourceOnline(
                  *this, static_cast<DfMp3_PlaySources>(replyArg));
              break;

            case 0x3A:
              _isOnline = true;
              T_NOTIFICATION_METHOD::OnPlaySourceInserted(
                  *this, static_cast<DfMp3_PlaySources>(replyArg));
              break;

            case 0x3B:
              _isOnline = true;
              T_NOTIFICATION_METHOD::OnPlaySourceRemoved(
                  *this, static_cast<DfMp3_PlaySources>(replyArg));
              break;

            case 0x40:
              T_NOTIFICATION_METHOD::OnError(*this, replyArg);
              return 0;

            default:
              // unknown/unsupported command reply
              break;
          }
        }
      } else {
        if (replyArg != 0) {
          T_NOTIFICATION_METHOD::OnError(*this, replyArg);
          if (_serial.available() == 0) {
            // -1 means an error occurred
            return -1;
          }
        }
      }
    } while (command != 0);

    return 0;
  }

  int32_t sendAndListenForReply(uint8_t command, uint16_t arg = 0, int retryCount = 3) {
    int32_t rep = -1;
    for (int i = 0; i < retryCount && rep == -1; i++) {
      sendPacket(command, arg);
      rep = listenForReply(command);
    }
    return rep;
  }
};