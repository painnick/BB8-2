import logging
from colorlog import ColoredFormatter
import time

import cv2
import mediapipe as mp
import numpy as np
import requests
from cv2 import ROTATE_180

host = 'http://192.168.5.18'
output_width = 640
output_height = 480

# Logger Settings
logger = logging.getLogger("main")
logger.setLevel(logging.DEBUG)
formatter = ColoredFormatter(
    "%(log_color)s[%(asctime)s] %(message)s",
    datefmt=None,
    reset=True,
    log_colors={
        'DEBUG':    'cyan',
        'INFO':     'white,bold',
        'INFOV':    'cyan,bold',
        'WARNING':  'yellow',
        'ERROR':    'red,bold',
        'CRITICAL': 'red,bg_white',
    },
    secondary_log_colors={},
    style='%'
)
stream_handler = logging.StreamHandler()
stream_handler.setFormatter(formatter)
logger.addHandler(stream_handler)

# HTTP Client
session = requests.Session()

# Window Settings
cv2.namedWindow('mp', cv2.WINDOW_NORMAL)
# cv2.setWindowProperty('mp', cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)

mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils

face_lost = time.time()
direction = 'none'
output = np.zeros((output_height, output_width, 3), np.uint8)


def capture():
  res = None
  try:
    res = session.get('%s/capture' % host)
  except requests.exceptions.ConnectionError as con_error:
    logger.error('%s', con_error)
    if output is not None:
      cv2.putText(output,
                  '%s' % con_error,
                  (10, 32),
                  cv2.FONT_HERSHEY_PLAIN,
                  2,
                  (0, 0, 255),
                  1,
                  cv2.LINE_8)
      cv2.imshow('mp', output)
  except requests.exceptions.RequestException as req_error:
    logger.error('%s', req_error)
    if output is not None:
      cv2.putText(output,
                  '%s' % req_error,
                  (10, 32),
                  cv2.FONT_HERSHEY_PLAIN,
                  2,
                  (0, 0, 255),
                  1,
                  cv2.LINE_8)
      cv2.imshow('mp', output)
  return res


def response_to_np(res):
  try:
    np_arr = np.array(bytearray(res.content), dtype=np.uint8)
  except Exception as err:
    logger.error('Read Error : %s', err)
    if output is not None:
      cv2.putText(output,
                  'Read Error : %s' % err,
                  (10, 32),
                  cv2.FONT_HERSHEY_PLAIN,
                  2,
                  (0, 0, 255),
                  1,
                  cv2.LINE_8)
      cv2.imshow('mp', output)
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
    if output is not None:
      cv2.putText(output,
                  '%s' % con_error,
                  (10, 32),
                  cv2.FONT_HERSHEY_PLAIN,
                  2,
                  (0, 0, 255),
                  1,
                  cv2.LINE_8)
      cv2.imshow('mp', output)
  except requests.exceptions.RequestException as req_error:
    logger.error('%s', req_error)
    if output is not None:
      cv2.putText(output,
                  '%s' % req_error,
                  (10, 32),
                  cv2.FONT_HERSHEY_PLAIN,
                  2,
                  (0, 0, 255),
                  1,
                  cv2.LINE_8)
      cv2.imshow('mp', output)


with mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.7) as face_detection:
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
    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    image = cv2.rotate(image, ROTATE_180)
    image = cv2.flip(image, 1)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = face_detection.process(image)

    # Draw the face detection annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    if results.detections:  # Found
      box = None
      for detection in results.detections:
        found = detection.location_data.relative_bounding_box
        if box is None:
          box = found
        elif found.width > box.width:
          box = found
      center = box.xmin + (box.width / 2)  # Calculate only the space represented within the screen
      mp_drawing.draw_detection(image, detection)
      if 0.3 > center or center > 0.7:
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
        cv2.imshow('mp', image)
        cv2.waitKey(1000 * 4)
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

    # output = cv2.resize(flipped, (640, 480), interpolation=cv2.INTER_CUBIC)
    cv2.imshow('mp', image)
    if cv2.waitKey(1) == 27:
      break
