#-*- coding: utf-8 -*-

import cv2
import cv2.cv as cv
import numpy as np

# 눈을 인식하기 위한 관련 상수이다.
# haar cascade를 통하여 얼굴 인식 후 눈을 인식하게 된다.
# 눈 또한 haar cascade를 통하여 인식하게 된다면 작업을 컴퓨터 환경에 따라서 처리하기 힘들다.
# 따라서 먼저 인식된 얼굴에서 비율을 통하여 찾도록 한다.
EYE_PERCENT_TOP = 31        # 눈이 얼굴(y축 기준)에서 얼마나 위쪽에 위치하는지 결정
EYE_PERCENT_SIDE = 18       # 눈이 얼굴 중앙(x축 기준)으로부터 얼마나 옆에 위치하는지 결정
EYE_PERCENT_HEIGHT = 13     # 눈의 높이
EYE_PERCENT_WIDTH = 25      # 눈의 너비

# 눈 인식 후 눈의 흰색 부분과 검정색 부분을 나누게 될 Threshold
# Gray Frame에서 Threshold 값보다 작은 값은 흰색(255)으로, 높은 값은 검정색(0)으로 이진화하게 된다.
EYE_THRESHOLD = 55
BINARY_WHITE = 255
BINARY_BLACK = 0

# Algorithm Parameters
THRESHOLD_SCALE = 3

# Window
MAIN_WINDOW_NAME = "main"
FACE_WINDOW_NAME = "face"
LEFT_EYE_WINDOW_NAME = "left eye"
RIGHT_EYE_WINDOW_NAME = "right eye"
FACE_CASCADE_NAME = "./haarcascade_frontalface_alt.xml"
EYE_CASCADE_NAME = "./haarcascade_eye_tree_eyeglasses.xml"
EYE_CASCADE = cv2.CascadeClassifier(EYE_CASCADE_NAME)
FACE_CASCADE = cv2.CascadeClassifier(FACE_CASCADE_NAME)


def main():
    cap = cv2.VideoCapture(0)
    while True:
        ret, img = cap.read()   # 화면을 캡처한다.
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)    # 화면을 흑백으로 변환한다.
        faces = FACE_CASCADE.detectMultiScale(gray, 1.1, 2, 0, (150, 150))  # 얼굴을 디텍팅 한다.
        for (x, y, w, h) in faces:  # 찾아낸 얼굴들에 대해서 아래 작업을 실행한다.
            cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 1)  # 얼굴에 사각형을 그린다.

        if len(faces) > 0:  # 얼굴이 하나 이상 존재한다면 아래 작업을 실행한다.
            find_eyes(gray, faces[0])   # find_eyes 함수를 호출한다. 아래 함수 참고

        cv2.imshow("gray", gray)    # 흑백화면 출력
        cv2.imshow("main", img)     # 컬러화면 출력


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
    left_eye_region = (int(w * (EYE_PERCENT_SIDE/100.0)), int(eye_region_top), \
                      int(eye_region_width), int(eye_region_height))
    # 오른쪽 눈의 경우에는 (프레임 전체 - 눈 너비 - 눈 위치 x축) 좌표에 위치한다.
    right_eye_region = (int(w - eye_region_width - w*(EYE_PERCENT_SIDE/100.0)), \
                       int(eye_region_top), int(eye_region_width), int(eye_region_height))
    # 눈을 그려주는 함수를 호출한다.
    find_eye_region(face_roi, left_eye_region)
    find_eye_region(face_roi, right_eye_region)

    # 눈깜빡임을 인식한다.
    # TODO: Thresold eye region to get eye pupil
    # then use HoughCircles to detect open/closed eye
    # Read more here: http://answers.opencv.org/question/32688/finding-the-center-of-eye-pupil/
    
    # 왼쪽 눈의 좌표 및 너비 및 높이 할당
    eye_left_x, eye_left_y, eye_left_width, eye_left_height = left_eye_region
    # 왼쪽 눈의 영상을 위에서 얻은 값들을 통해 얼굴 전체 영상에서 읽어온다.
    left_eye_roi = face_roi[eye_left_y:eye_left_y+eye_left_height, \
                            eye_left_x:eye_left_x+eye_left_width]

    # 오른쪽 눈의 좌표 및 너비 및 높이 할당
    eye_right_x, eye_right_y, eye_right_width, eye_right_height = right_eye_region
    # 오른쪽 눈의 영상을 위에서 얻은 값들을 통해 얼굴 전체 영상에서 읽어온다.
    right_eye_roi = face_roi[eye_right_y:eye_right_y+eye_right_height, \
                            eye_right_x:eye_right_x+eye_right_width]

    # 이진화 된 눈에 대해서 동공과 그 외 부분을 분리하기 위해서 정제 과정을 거친다.
    kernel = np.ones((9, 9), np.uint8)  # 배열을 생성
    # 눈의 영상에서 EYE_THRESHOLD를 초과하면 흰색(BINARY_WHITE, 255)로, 아니면 (BINARY_BLACK, 0)으로 만든다. (이진화)
    (_, thresh_left) = cv2.threshold(left_eye_roi, EYE_THRESHOLD, BINARY_WHITE, cv2.THRESH_BINARY_INV)
    # 팽창
    #thresh_left = cv2.dilate(thresh_left, kernel, iterations=1)
    # 침식
    #thresh_left = cv2.erode(thresh_left, kernel, iterations=1)
    # 가우시안 블러
    #thresh_left = cv2.GaussianBlur(thresh_left, (9, 9), 0)
    # 리사이즈
    #thresh_left = cv2.resize(thresh_left, (0, 0), fx=THRESHOLD_SCALE, fy=THRESHOLD_SCALE)
    cv2.imshow("thresh_left_eye_roi", thresh_left)

    # 오른쪽 눈에 대해서도 왼쪽 눈과 같은 과정을 거친다.
    (_, thresh_right) = cv2.threshold(right_eye_roi, EYE_THRESHOLD, BINARY_WHITE, cv2.THRESH_BINARY_INV)
    thresh_right = cv2.dilate(thresh_right, kernel, iterations=1)
    thresh_right = cv2.erode(thresh_right, kernel, iterations=1)
    thresh_right = cv2.GaussianBlur(thresh_right, (9, 9), 0)
    thresh_right = cv2.resize(thresh_right, (0, 0), fx=THRESHOLD_SCALE, fy=THRESHOLD_SCALE)
    cv2.imshow("thresh_right_eye_roi", thresh_right)

    # 정제된 눈의 영상에 대해서 다음 과정을 거친다.
    # 동공을 찾기 위해서 원을 찾기 위한 HoughCircles 함수를 사용한다.
    circles_left = cv2.HoughCircles(thresh_left, cv.CV_HOUGH_GRADIENT, 2, 200,
                                    param1=30, param2=10, minRadius=15, maxRadius=25)
    circles_right = cv2.HoughCircles(thresh_right, cv.CV_HOUGH_GRADIENT, 2, 200,
                                     param1=30, param2=10, minRadius=15, maxRadius=25)

    # 찾은 동공에 대해서 원을 그린다.
    if circles_left is not None:
        circles_left = np.uint16(np.around(circles_left))
        for (x, y, r) in circles_left[0, :]:
            cv2.circle(left_eye_roi, (x/THRESHOLD_SCALE, y/THRESHOLD_SCALE), \
                        r/THRESHOLD_SCALE, (0, 0, 0), 2)
    if circles_right is not None:
        circles_right = np.uint16(np.around(circles_right))
        for (x, y, r) in circles_right[0, :]:
            cv2.circle(right_eye_roi, (x/THRESHOLD_SCALE, y/THRESHOLD_SCALE), \
                        r/THRESHOLD_SCALE, (0, 0, 0), 2)

    cv2.imshow("left_eye_roi", left_eye_roi)
    cv2.imshow("right_eye_roi", right_eye_roi)
    cv2.resizeWindow("left_eye_roi", 300, 300)
    cv2.resizeWindow("right_eye_roi", 300, 300)


def find_eye_region(face, eye):
    eye_x, eye_y, eye_width, eye_height = eye
    cv2.rectangle(face, (eye_x, eye_y), (eye_x + eye_width, eye_y + eye_height), (255, 0, 0), 1)


if __name__ == '__main__':
    main()

# 함수들 설명
# 1. cv2.HoughCircles(image, method, dp, minDist, param1, param2, minRadius, maxRadius)
# dp – Inverse ratio of the accumulator resolution to the image resolution. For example, if dp=1 , the accumulator has the same resolution as the input image. If dp=2 , the accumulator has half as big width and height.
# minDist – Minimum distance between the centers of the detected circles. If the parameter is too small, multiple neighbor circles may be falsely detected in addition to a true one. If it is too large, some circles may be missed.
# param1 – First method-specific parameter. In case of CV_HOUGH_GRADIENT , it is the higher threshold of the two passed to the Canny() edge detector (the lower one is twice smaller).
# param2 – Second method-specific parameter. In case of CV_HOUGH_GRADIENT , it is the accumulator threshold for the circle centers at the detection stage. The smaller it is, the more false circles may be detected. Circles, corresponding to the larger accumulator values, will be returned first.
# minRadius – Minimum circle radius.
# maxRadius – Maximum circle radius.

