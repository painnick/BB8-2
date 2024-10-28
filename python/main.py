import logging
import sys
import time

import cv2
import mediapipe as mp
import numpy as np
import requests
from PyQt5 import QtGui, uic
from PyQt5.QtCore import pyqtSignal, pyqtSlot, Qt, QThread
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QApplication, QDialog
from colorlog import ColoredFormatter
from cv2 import ROTATE_180

# HTTP Client
host = 'http://192.168.5.18'
session = requests.Session()
output_width = 320
output_height = 240

find_faces = False

face_lost = time.time()
direction = 'none'


def capture():
  res = None
  try:
    res = session.get('%s/capture' % host)
  except requests.exceptions.ConnectionError as con_error:
    logger.error('%s', con_error)
  except requests.exceptions.RequestException as req_error:
    logger.error('%s', req_error)
  return res


def response_to_np(res):
  try:
    np_arr = np.array(bytearray(res.content), dtype=np.uint8)
  except Exception as err:
    logger.error('Read Error : %s', err)
    return None
  return np_arr


def turn(dir, found):
  try:
    if dir != 'none':
      if found:
        logger.info("%s - %s", "found", dir)
      else:
        logger.debug("%s - %s", "Not found", dir)
    session.get('%s/motor' % host, params={'dir': dir, 'found': found})
    # time.sleep(sleep_sec)
  except requests.exceptions.ConnectionError as con_error:
    logger.error('%s', con_error)
  except requests.exceptions.RequestException as req_error:
    logger.error('%s', req_error)


class VideoThread(QThread):
  change_pixmap_signal = pyqtSignal(np.ndarray)

  def run(self):

    global find_faces
    global direction
    global face_lost

    mp_face_detection = mp.solutions.face_detection
    mp_drawing = mp.solutions.drawing_utils

    with mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.5) as face_detection:
      while True:
        current_time = time.time()
        if current_time - face_lost < 1.0:
          continue

        response = capture()
        if response is None:
          if cv2.waitKey(1000 * 3) == 27:
            break
          continue

        np_from_response = response_to_np(response)
        if np_from_response is None:
          if cv2.waitKey(1000 * 2) == 27:
            break
          continue

        image = cv2.imdecode(np_from_response, -1)

        image.flags.writeable = False
        image = cv2.rotate(image, ROTATE_180)
        image = cv2.flip(image, 1)

        if find_faces:
          # To improve performance, optionally mark the image as not writeable to
          # pass by reference.

          image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
          results = face_detection.process(image)

          # Draw the face detection annotations on the image.
          image.flags.writeable = True
          image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
          if results.detections:
            box = None
            for detection in results.detections:
              found = detection.location_data.relative_bounding_box
              if box is None:
                box = found
              elif found.width > box.width:
                box = found
            center = box.xmin + (box.width / 2)  # Calculate only the space represented within the screen
            mp_drawing.draw_detection(image, detection)
            if 0.4 > center or center > 0.6:
              if center < 0.5:
                direction = 'right'
                turn(direction, True)
                time.sleep(0.01)
                turn('none', True)  # Stop
              else:
                direction = 'left'
                turn(direction, True)
                time.sleep(0.01)
                turn('none', True)  # Stop
            else:
              logger.info('Found(Center)')
              turn('none', True)
              cv2.putText(image,
                          'Found!',
                          (10, 32),
                          cv2.FONT_HERSHEY_PLAIN,
                          1,
                          (0, 0, 255),
                          2,
                          cv2.LINE_8)
              # cv2.waitKey(1000 * 1)
            face_lost = time.time()
          else:  # Lost
            past_time = current_time - face_lost
            if 2 < past_time < 5:
              logger.debug('Finding...')
              if direction == 'none':
                direction = 'right'
              turn(direction, False)
              time.sleep(0.02)
              turn('none', False)
            else:
              time.sleep(0.01)

        self.change_pixmap_signal.emit(image)


form_class = uic.loadUiType("app.ui")[0]


class App(QDialog, form_class):
  def __init__(self):
    super().__init__()
    self.setupUi(self)

    self.find_button.clicked.connect(self.find_button_clicked)
    self.left_button.clicked.connect(self.left_button_clicked)
    self.right_button.clicked.connect(self.right_button_clicked)

    # create the video capture thread
    self.thread = VideoThread()
    # connect its signal to the update_image slot
    self.thread.change_pixmap_signal.connect(self.update_image)
    # start the thread
    self.thread.start()

  @pyqtSlot(np.ndarray)
  def update_image(self, cv_img):
    """Updates the image_label with a new opencv image"""
    qt_img = self.convert_cv_qt(cv_img)
    self.image_label.setPixmap(qt_img)

  def convert_cv_qt(self, cv_img):
    """Convert from an opencv image to QPixmap"""
    rgb_image = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
    h, w, ch = rgb_image.shape
    bytes_per_line = ch * w
    convert_to_Qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_RGB888)
    p = convert_to_Qt_format.scaled(800, 600, Qt.KeepAspectRatio)
    return QPixmap.fromImage(p)

  def find_button_clicked(self):
    global find_faces
    find_faces = not find_faces

  def left_button_clicked(self):
    turn('left', False)
    time.sleep(0.05)
    turn('none', True)  # Stop

  def right_button_clicked(self):
    turn('right', False)
    time.sleep(0.05)
    turn('none', True)  # Stop


if __name__ == "__main__":
  # Logger Settings
  logger = logging.getLogger("main")
  logger.setLevel(logging.DEBUG)
  formatter = ColoredFormatter(
    "%(log_color)s[%(asctime)s] %(message)s",
    datefmt=None,
    reset=True,
    log_colors={
      'DEBUG': 'cyan',
      'INFO': 'white,bold',
      'INFOV': 'cyan,bold',
      'WARNING': 'yellow',
      'ERROR': 'red,bold',
      'CRITICAL': 'red,bg_white',
    },
    secondary_log_colors={},
    style='%'
  )
  stream_handler = logging.StreamHandler()
  stream_handler.setFormatter(formatter)
  logger.addHandler(stream_handler)

  app = QApplication(sys.argv)
  a = App()
  a.show()
  sys.exit(app.exec_())
