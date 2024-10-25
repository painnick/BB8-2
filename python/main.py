import json
import socket
import time
from urllib import response
import cv2
from cv2 import ROTATE_180
import mediapipe as mp
import urllib.request
from urllib.error import HTTPError, URLError
import numpy as np
import logging
import time

logger = logging.getLogger("main")
logger.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
stream_handler = logging.StreamHandler()
stream_handler.setFormatter(formatter)
logger.addHandler(stream_handler)

cv2.namedWindow('mp', cv2.WINDOW_NORMAL)
# cv2.setWindowProperty('mp', cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)

mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils

host = 'http://192.168.5.18'

face_found = time.time()
face_lost = time.time()
direction = 'none'

# For webcam input:
with mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.7) as face_detection:
  while True:
    try:
      img_resp = urllib.request.urlopen(url=host + '/capture', timeout=1)
    except (HTTPError, URLError) as error:
      logging.error('Data not retrieved because %s', error)
      if cv2.waitKey(1000) == 27:
        break
      continue
    except ConnectionResetError:
      logger.error('Connection Reset!')
      if cv2.waitKey(1000) == 27:
        break
      continue

    try:
      imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    except socket.timeout:
      logger.error('Timeout!')
      if cv2.waitKey(1000) == 27:
        break
      continue

    image = cv2.imdecode(imgnp, -1)

    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    image = cv2.rotate(image, ROTATE_180)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = face_detection.process(image)

    # Draw the face detection annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    if results.detections:  # Found
      face_found = time.time()
      for detection in results.detections:
        box = detection.location_data.relative_bounding_box
        center = box.xmin + (box.width / 2)  # Calculate only the space represented within the screen
        mp_drawing.draw_detection(image, detection)
        if 0.3 > center or center > 0.7:
          if center > 0.5:
            direction = 'right'
            logger.debug(direction)
            urllib.request.urlopen(host + '/motor?dir=%s&found=true' % (direction))
            time.sleep(0.3)
          else:
            direction = 'left'
            logger.debug(direction)
            urllib.request.urlopen(host + '/motor?dir=%s&found=true' % (direction))
            time.sleep(0.3)
        else:
          logger.debug('Found(Center)')
          urllib.request.urlopen(host + '/motor?dir=%s&found=true' % (direction))
          cv2.imshow('mp', cv2.flip(image, 1))
          cv2.waitKey(1000 * 5)
      face_lost = time.time()  # Reset lost timing
    else:  # Lost
      current_time = time.time()
      if current_time - face_lost > 2:
        logger.debug('Lost')
        logger.debug(direction)
        face_lost = time.time()
        urllib.request.urlopen(host + '/motor?dir=%s&found=false' % (direction))
        time.sleep(0.1)

    # Flip the image horizontally for a selfie-view display.
    cv2.imshow('mp', cv2.flip(image, 1))
    if cv2.waitKey(1) == 27:
      break
