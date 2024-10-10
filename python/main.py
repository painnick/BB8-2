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

logger = logging.getLogger("main")
logger.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
stream_handler = logging.StreamHandler()
stream_handler.setFormatter(formatter)
logger.addHandler(stream_handler)

mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils

host = 'http://192.168.5.18'

face_found = time.time()
face_lost = time.time()
direction = 'none'

# For webcam input:
with mp_face_detection.FaceDetection(
    model_selection=0, min_detection_confidence=0.6) as face_detection:
  while True:
    try:
      img_resp = urllib.request.urlopen(url=host + '/capture', timeout=1)
    except (HTTPError, URLError) as error:
      logging.error('Data not retrieved because %s', error)
      continue
    except ConnectionResetError:
      logger.error('Connection Reset!')
      continue

    try:
      imgnp = np.array(bytearray(img_resp.read()),dtype=np.uint8)
    except socket.timeout:
      logger.error('Timeout!')
      continue

    image = cv2.imdecode(imgnp,-1)

    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    image = cv2.rotate(image, ROTATE_180)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = face_detection.process(image)

    # Draw the face detection annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    if results.detections: # Found
      face_found = time.time()
      for detection in results.detections:
        box = detection.location_data.relative_bounding_box
        center = box.xmin + (box.width / 2)
        mp_drawing.draw_detection(image, detection)
        if 0.4 > center or center > 0.6:
          if center > 0.5:
            direction = 'right'
            urllib.request.urlopen(host + '/motor?dir=%s&found=true' %(direction))
            # logger.debug(direction)
          else:
            direction = 'left'
            urllib.request.urlopen(host + '/motor?dir=%s&found=true' %(direction))
            # logger.debug(direction)
        #else:
        #    direction = 'none'
        #    urllib.request.urlopen(host + '/motor?dir=%s&found=true' % (direction))
      face_lost = time.time()
    else: # Lost
      current_time = time.time()
      if current_time - face_lost > 2:
        face_lost = time.time()
        urllib.request.urlopen(host + '/motor?dir=%s&found=false' %(direction))
        logger.debug(direction)

    # Flip the image horizontally for a selfie-view display.
    #cv2.imshow('MediaPipe Face Detection', cv2.flip(image, 1))
    #if cv2.waitKey(5) & 0xFF == 27:
    #  break
