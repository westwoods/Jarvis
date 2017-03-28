#-*- coding: utf-8 -*-

import cv2
import cv2.cv as cv
import numpy as np
from matplotlib import pyplot as plt

# 눈을 인식하기 위한 관련 상수이다.
# haar cascade를 통하여 얼굴 인식 후 눈을 인식하게 된다.
# 눈 또한 haar cascade를 통하여 인식하게 된다면 작업을 컴퓨터 환경에 따라서 처리하기 힘들다.
# 따라서 먼저 인식된 얼굴에서 비율을 통하여 찾도록 한다.
EYE_PERCENT_TOP = 31        # 눈이 얼굴(y축 기준)에서 얼마나 위쪽에 위치하는지 결정
EYE_PERCENT_SIDE = 18       # 눈이 얼굴 중앙(x축 기준)으로부터 얼마나 옆에 위치하는지 결정
EYE_PERCENT_HEIGHT = 15     # 눈의 높이
EYE_PERCENT_WIDTH = 28      # 눈의 너비

# 색상 관련 상수
BINARY_WHITE = 255
BINARY_BLACK = 0

# 화면 및 cascade path
MAIN_WINDOW_NAME = "main"
FACE_WINDOW_NAME = "face"
LEFT_EYE_WINDOW_NAME = "left eye"
RIGHT_EYE_WINDOW_NAME = "right eye"
FACE_CASCADE_NAME = "./haarcascade_frontalface_alt.xml"
EYE_CASCADE_NAME = "./haarcascade_eye_tree_eyeglasses.xml"
EYE_CASCADE = cv2.CascadeClassifier(EYE_CASCADE_NAME)
FACE_CASCADE = cv2.CascadeClassifier(FACE_CASCADE_NAME)

# 클래스 이름
CLASS_NAME = "open"
# 데이터 카운트
DATA_COUNT = 0

def main():
    cap = cv2.VideoCapture(0)
    while True:
        ret, img = cap.read()   # 화면을 캡처한다.
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)    # 화면을 흑백으로 변환한다.
        faces = FACE_CASCADE.detectMultiScale(gray, 1.1, 2, 0, (150, 150))  # 얼굴을 디텍팅 한다.
        for (x, y, w, h) in faces:  # 찾아낸 얼굴들에 대해서 아래 작업을 실행한다.
            cv2.rectangle(gray, (x, y), (x + w, y + h), (255, 0, 0), 1)  # 얼굴에 사각형을 그린다.

        if len(faces) > 0:  # 얼굴이 하나 이상 존재한다면 아래 작업을 실행한다.
            find_eyes(gray, faces[0])   # find_eyes 함수를 호출한다. 아래 함수 참고

        cv2.imshow("gray", gray)    # 흑백화면 출력
        #cv2.imshow("main", img)     # 컬러화면 출력

        k = cv2.waitKey(10) & 0xFF
        if k == 27:
            break
        elif k == ord('f'):
            cv2.imwrite("frame.png", frame)
            cv2.flip(frame, frame, 1)
    cv2.destroyAllWindows()


def find_eyes(gray, face):  # 위에서 흑백프레임과 얼굴을 넘겨받음.
    (x, y, w, h) = face # 얼굴의 위치와 넓이와 높이를 얻는다.
    face_roi = gray[y:y+h, x:x+h]   # 얼굴 부분만 얻어온다.
    
    # 눈 부분 인식은 얼굴 영역에서 눈이 대략 어느정도 차지하는지로 결정한다.
    eye_region_width = w * (EYE_PERCENT_WIDTH/100.0)    # 눈 너비를 계산한다. 위의 상수 참고
    eye_region_height = w * (EYE_PERCENT_HEIGHT/100.0)  # 눈 높이를 계산한다. 위의 상수 참고
    eye_region_top = h * (EYE_PERCENT_TOP/100.0)    # 눈 의 y 위치를 대략 결정한다. 위의 상수 값을 낮추면 얼굴에서 위쪽에 위치
    
    # EYE_PERCENT_SIDE는 얼마나 눈이 얼굴 중심으로부터 옆으로 가는지 결정한다.
    #왼쪽 눈의 경우에는 얼굴 영역에서 왼쪽에서 x축 좌표에 위치한다.
    leftEyeRegion = (int(w * (EYE_PERCENT_SIDE/100.0)), int(eye_region_top), \
                      int(eye_region_width), int(eye_region_height))
    # 오른쪽 눈의 경우에는 (프레임 전체 - 눈 너비 - 눈 위치 x축) 좌표에 위치한다.
    RightEyeRegion = (int(w - eye_region_width - w*(EYE_PERCENT_SIDE/100.0)), \
                       int(eye_region_top), int(eye_region_width), int(eye_region_height))
    # 눈을 그려주는 함수를 호출한다.
    ShowEyeRegion(face_roi, leftEyeRegion)
    ShowEyeRegion(face_roi, RightEyeRegion)

    # 눈깜빡임을 인식한다.
    # 왼쪽 눈의 좌표 및 너비 및 높이 할당
    eye_left_x, eye_left_y, eye_left_width, eye_left_height = leftEyeRegion
    # 왼쪽 눈의 영상을 위에서 얻은 값들을 통해 얼굴 전체 영상에서 읽어온다.
    frameOfLeftEye = face_roi[eye_left_y:eye_left_y+eye_left_height, \
                            eye_left_x:eye_left_x+eye_left_width]

    # 오른쪽 눈의 좌표 및 너비 및 높이 할당
    eye_right_x, eye_right_y, eye_right_width, eye_right_height = RightEyeRegion
    # 오른쪽 눈의 영상을 위에서 얻은 값들을 통해 얼굴 전체 영상에서 읽어온다.
    frameOfRightEye = face_roi[eye_right_y:eye_right_y+eye_right_height, \
                            eye_right_x:eye_right_x+eye_right_width]
    
    cv2.namedWindow("left_eye", cv2.WINDOW_NORMAL)
    cv2.imshow("left_eye", frameOfLeftEye)
    cv2.namedWindow("right_eye", cv2.WINDOW_NORMAL)
    cv2.imshow("right_eye", frameOfRightEye)

    global DATA_COUNT
    fileName = CLASS_NAME + "." + DATA_COUNT.__str__() + ".jpg"
    print DATA_COUNT
    DATA_COUNT = DATA_COUNT + 1
    print fileName
    cv2.imwrite(fileName, frameOfLeftEye)

# 입력 : 얼굴프레임과 눈 영역의 위치 정보
# 동작 : 얼굴 프레임에 눈 위치에 맞게 사각형을 그린다.
# 리턴 : x
def ShowEyeRegion(frameOfFace, positionOfEye):
    eye_x, eye_y, eye_width, eye_height = positionOfEye
    cv2.rectangle(frameOfFace, (eye_x, eye_y), (eye_x + eye_width, eye_y + eye_height), (255, 0, 0), 1)

def printEyeStatus(frame, eyeStatus):
    font = cv2.FONT_HERSHEY_SIMPLEX
    if(eyeStatus == EYE_OPEN):
        cv2.putText(frame,'EYE_OPEN',(50,50), font, 2,(0,0,0),2)
    else:
        cv2.putText(frame,'EYE_CLOSED',(50,50), font, 2,(0,0,0),2)

if __name__ == '__main__':
    main()
