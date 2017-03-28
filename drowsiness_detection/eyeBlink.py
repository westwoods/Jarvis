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

# 눈 인식 후 눈의 흰색 부분과 검정색 부분을 나누게 될 Threshold
# Gray Frame에서 Threshold 값보다 작은 값은 흰색(255)으로, 높은 값은 검정색(0)으로 이진화하게 된다.
EYE_THRESHOLD = 50 # 맥용 카메라
#EYE_THRESHOLD = 170 # 마이크로소프트 카메라

# 색상 관련 상수
BINARY_WHITE = 255
BINARY_BLACK = 0

# 깜빡임을 인식하는 Threshold
BLINK_THRESHOLD = 150    # 맥용 카메라
#BLINK_THRESHOLD = 9000    # 마이크로소프트 카메라

# 눈의 상태 상수
EYE_OPEN = 0
EYE_CLOSE = 1

# 화면 리사이즈 상수, 값이 커지면 더 크게 출력한다.
THRESHOLD_SCALE = 3

# 화면 및 cascade path
MAIN_WINDOW_NAME = "main"
FACE_WINDOW_NAME = "face"
LEFT_EYE_WINDOW_NAME = "left eye"
RIGHT_EYE_WINDOW_NAME = "right eye"
FACE_CASCADE_NAME = "./haarcascade_frontalface_alt.xml"
EYE_CASCADE_NAME = "./haarcascade_eye_tree_eyeglasses.xml"
EYE_CASCADE = cv2.CascadeClassifier(EYE_CASCADE_NAME)
FACE_CASCADE = cv2.CascadeClassifier(FACE_CASCADE_NAME)

# OTSU THRESHOLD 값이다. 이 값을 global로 저장해두었다가 이진화를 하는 곳에 사용한다.
OTSU_THRESHOLD = 0
# OTSU의 RESET 타이밍을 조절
OTSU_RESET = 10
# OTSU TRESHOLD를 새롭게 구하는 타이밍이다. 실제상황에서 밝기가 계속 바뀌기 때문에 특정 타이밍마다 OTSU_THRESHOLD를 갱신한다.
OTSU_COUNT = OTSU_RESET
# 눈을 감았을 때와 OTSU를 다시 계산할 타이밍이 겹치는 경우를 방지하기 위해 OTSU_THRESHOLD가 특정 값을 넘어가지 않도록 한다.
OTSU_LIMIT = 90


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

    # 이진화 된 눈에 대해서 동공과 그 외 부분을 분리하기 위해서 정제 과정을 거친다.
    kernel = np.ones((9, 9), np.uint8)  
    # Gaussian filtering
    frameOfLeftEye = cv2.GaussianBlur(frameOfLeftEye,(5,5),0)
    # Otsu와 관련된 작업
    Otsu(frameOfLeftEye)
    # 이진화
    (_, filteredLeftEye) = cv2.threshold(frameOfLeftEye, OTSU_THRESHOLD, BINARY_WHITE, cv2.THRESH_BINARY_INV)
    # 팽창
    #filteredLeftEye = cv2.dilate(filteredLeftEye, kernel, iterations=1)
    # 침식
    #filteredLeftEye = cv2.erode(filteredLeftEye, kernel, iterations=1)
    # 가우시안 블러
    #filteredLeftEye = cv2.GaussianBlur(filteredLeftEye, (9, 9), 0)
    # 리사이즈
    #filteredLeftEye = cv2.resize(filteredLeftEye, (0, 0), fx=THRESHOLD_SCALE, fy=THRESHOLD_SCALE)
    cv2.namedWindow("filteredLeftEye_eye", cv2.WINDOW_NORMAL)
    cv2.imshow("filteredLeftEye_eye", filteredLeftEye)

    # 오른쪽 눈에 대해서도 왼쪽 눈과 같은 과정을 거친다.
    (_, filteredRightEye) = cv2.threshold(frameOfRightEye, EYE_THRESHOLD, BINARY_WHITE, cv2.THRESH_BINARY_INV)
    cv2.namedWindow("1", cv2.WINDOW_NORMAL)
    cv2.imshow("1", filteredRightEye)
    filteredRightEye = cv2.dilate(filteredRightEye, kernel, iterations=1)
    cv2.namedWindow("2", cv2.WINDOW_NORMAL)
    cv2.imshow("2", filteredRightEye)
    #filteredRightEye = cv2.erode(filteredRightEye, kernel, iterations=1)
    filteredRightEye = cv2.GaussianBlur(filteredRightEye, (9, 9), 0)
    cv2.namedWindow("3", cv2.WINDOW_NORMAL)
    cv2.imshow("3", filteredRightEye)
    filteredRightEye = cv2.resize(filteredRightEye, (0, 0), fx=THRESHOLD_SCALE, fy=THRESHOLD_SCALE)
    #cv2.namedWindow("filteredRightEye", cv2.WINDOW_NORMAL)
    #cv2.imshow("filteredRightEye", filteredRightEye)

    # 흰색을 카운팅 한다.
    countOfWhite = CountWhite(filteredLeftEye)
    print countOfWhite
    # 눈이 감김 여부를 알아낸다.
    eyeStatus = CheckEyeClose(countOfWhite)
    # 눈의 상태를 콘솔에 출력한다.
    #if(eyeStatus == EYE_OPEN):
    #    print ("eye open")
    #else:
    #    print ("eye closed")
    #화면에 뿌려줌
    printEyeStatus(gray,eyeStatus)

    #히스토그램
    #hist_mask = cv2.calcHist([filteredLeftEye],[0],filteredLeftEye,[256],[0,256])
    #print hist_mask
    #plt.hist(hist_mask.ravel(),256,[0,256])
    #plt.show()


    cv2.namedWindow("left_eye", cv2.WINDOW_NORMAL)
    cv2.imshow("left_eye", frameOfLeftEye)
    cv2.namedWindow("right_eye", cv2.WINDOW_NORMAL)
    cv2.imshow("right_eye", frameOfRightEye)

# 입력 : 얼굴프레임과 눈 영역의 위치 정보
# 동작 : 얼굴 프레임에 눈 위치에 맞게 사각형을 그린다.
# 리턴 : x
def ShowEyeRegion(frameOfFace, positionOfEye):
    eye_x, eye_y, eye_width, eye_height = positionOfEye
    cv2.rectangle(frameOfFace, (eye_x, eye_y), (eye_x + eye_width, eye_y + eye_height), (255, 0, 0), 1)

# 입력 : 프레임
# 동작 : 프레임에서 흰 색상의 수를 카운트하여 리턴한다.
# 출력 : 흰 색상의 수
def CountWhite(frameOfEye):
    # 흰색을 카운팅 한다.
    rangeOfColor = cv2.inRange(frameOfEye, BINARY_WHITE - 5, BINARY_WHITE)
    countOfWhite = cv2.countNonZero(rangeOfColor)
    return countOfWhite

# 입력 : 흰 색상의 수
# 동작 : 흰 색상의 수가 Threshold보다 작다면 True, 아니면 False를 리턴
# 출력 : True(눈이 감김), False(눈이 감기지 않음)
def CheckEyeClose(countOfWhite):
    if(countOfWhite < BLINK_THRESHOLD):
        return EYE_CLOSE
    else:
        return EYE_OPEN

# 입력 : 눈의 프레임
# 동작 : OTSU_COUNT 값을 OTSU_RESET과 비교하면서 같아지면 Otsu threshold를 새롭게 구해 OTSU_THRESHOLD에 저장, 같지 않다면 OTSU_COUNT 증가
# 출력 : X
def Otsu(frameOfEye):
    global OTSU_THRESHOLD
    global OTSU_COUNT
    global OTSU_LIMIT

    if(OTSU_COUNT == OTSU_RESET):
        print 'otsu 갱신'
        # otsu_value에는 Otsu threshold의 값이 저장됨.
        otsu_value, otsu = cv2.threshold(frameOfEye,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
        print otsu_value
        if(otsu_value > OTSU_LIMIT):
            print '눈을 감았을 때 OTSU를 계산했다. 이전의 THRESHOLD를 재사용한다.'
            OTSU_COUNT = 0
        else:
            OTSU_THRESHOLD = otsu_value # 값을 저장하고
            OTSU_COUNT = 0 # 카운트는 0으로 리셋
    else:
        OTSU_COUNT = OTSU_COUNT + 1

def printEyeStatus(frame, eyeStatus):
    font = cv2.FONT_HERSHEY_SIMPLEX
    if(eyeStatus == EYE_OPEN):
        cv2.putText(frame,'EYE_OPEN',(50,50), font, 2,(0,0,0),2)
    else:
        cv2.putText(frame,'EYE_CLOSED',(50,50), font, 2,(0,0,0),2)

if __name__ == '__main__':
    main()
