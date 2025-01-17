# Bandai BB-8

## 주요 기능

- 음성 인식
- 얼굴 추적(자체 AP, 현재 Android만 지원)
- 헤드 움직임
- LED
- 사운드 재생
- Bluetooth를 이용한 제어

## 주요 부품

- Head 파트
  - ESP32CAM + ov2640(Wide)
  - DC Motor(6V 100RPM) + DRV8833 Motor driver
  - ShiftRegister(74HC595N)
  - LEDs
  - Custom PCB #1
  - 3D printed Motor Guide
- Body 파트
  - ESP32(nodeMCU) 
  - ShiftRegister(74HC595N) x 2
  - LEDs
  - Hall sensor
  - Custom PCB #2
  - Voltage Step Down module(5v-to-3v) for LEDs
  - dfplayer
- Slip Ring
- VC-02(Voice Recognition module)

## 주요 기술

- 음성 인식 기능(vc-02)
  - 제품 링크
    - 국내 : https://www.devicemart.co.kr/goods/view?no=14907525
    - 알리 : https://ko.aliexpress.com/item/1005005316033953.html
  - 이전 버전인 vc-01은 "학습한 소리"에만 반응. 타인의 목소리에 대응하는 것이 어려움
  - vc-02는 영어와 중국어를 지원
  - 대상 문장을 자체 Web 설정에서 원하는 문장을 설정 가능
    - http://voice.ai-thinker.com/
  - 소스 코드 및 설치 바이너리가 제공
    - 코드 수정 및 빌드를 통해 추가 기능 개발 가능
  - Body에 설치된 ESP32와 Serial 통신
- 얼굴 인식([Google Media Pipe](https://ai.google.dev/edge/mediapipe/solutions/vision/face_detector), Camera, HttpServer)
  - ESP32CAM의 Camera 기능과 WebServer 기능을 이용하여 Camera Server를 구현
  - ESP32CAM의 자체 AP(Access Point) 기능을 이용하여 외부에서도 접속 가능하도록 처리
  - Android 또는 기타 클라이언트에서 자체 AP에 연결하고, HTTP로 Camera Server에 연결
  - Camera Server에서 읽어온 이미지를 Google Media Pipe로 분석하여 얼굴 존재 여부를 판단
  - Camera Server의  특정 URL을 호출하여  Head 파트에 설치된 ESP32CAM이 Body 파트에 설치된 ESP32에 Serial 통신으로 모터 이동을 지시
  - Android 또는 기타 클라이언트는 얼굴을 화면 중앙에 위치할 때까지 반복

## 도면

Fritzing에서 표시할 수 없는 일부 부분들이 생략 또는 변경
![image](https://github.com/user-attachments/assets/ae9096a7-fa22-453b-9071-b44e7bf3581a)
