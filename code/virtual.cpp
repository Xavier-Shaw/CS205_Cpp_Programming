//
// Created by 89737 on 2022/6/17.
//

#include "iostream"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <conio.h>
#include "opencv2/imgcodecs.hpp"

#define CVUI_IMPLEMENTATION

#include "./cvui.h"
#include "windows.h"


using namespace std;
using namespace cv;

Mat kernel, img, imgHSV, imgYCrCb, imgBlur, img_final, range_HSV, range_YCrCb;
vector<Mat> channels;
vector<Point> points;
Point startPoint;
Point mousePoint;
bool begin_draw = false;
bool start_draw = false;
bool end_draw = false;
bool do_operate = false;
bool operation_check = false;

string window = "OpenCV";

// 0: 绘画图形->检测图形  1: 绘画图形->检测操作
int MODE = 0;

int hmin = 106, smin = 12, vmin = 97;
int hmax = 255, smax = 118, vmax = 255;

int Ymin = 62, CRmin = 102, CBmin = 108;
int Ymax = 255, CRmax = 155, CBmax = 153;


Scalar GREEN = Scalar(0,255,0);
Scalar RED = Scalar(0,0,255);
Scalar BLUE = Scalar(255,0,0);
Scalar PURPLE = Scalar(255,0,255);
Scalar ORANGE = Scalar(70,183,239);

void let_Mouse_Move(int x, int y) {
    double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;//获取屏幕分辨率宽度
    double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;//获取屏幕分辨率高度
    double fx = x * (65535.0f / fScreenWidth);
    double fy = y * (65535.0f / fScreenHeight);
    INPUT Input = {0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    SendInput(1, &Input, sizeof(INPUT));
}

void let_Mouse_Click() {
    INPUT Input = {0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    cout << "Click" << endl;
}

void let_Mouse_DoubleClick() {
    INPUT Input = {0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    cout << "Double Click" << endl;
}

void let_Mouse_MidClick() {
    INPUT Input = {0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    cout << "Middle Click" << endl;
}

void let_Mouse_RightClick() {
    INPUT Input = {0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &Input, sizeof(INPUT));
    cout << "Right Click" << endl;
}

Point getContours() {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;


    findContours(img_final, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    //角点向量，仅存储轮廓中的角点
    vector<vector<Point>> conPoly(contours.size());
    //存储矩形边界框向量
    vector<Rect> boundRectangle(contours.size());

    Point center = Point(-1, -1);

    for (int i = 0; i < contours.size(); i++) {
        //先找到每个轮廓的面积
        int area = contourArea(contours[i]);


        //图形类型
        string objectType;

        //如果面积在可提取范围内，则提取对应轮廓
        if (area > 8000) {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            boundRectangle[i] = boundingRect(conPoly[i]);

//            drawContours(img, conPoly, i, Scalar(255,0,255), 2);
            rectangle(img, boundRectangle[i].tl(), boundRectangle[i].br(), Scalar(0, 255, 0), 4);

//            以矩形中心为画笔
//            center = Point(boundRectangle[i].tl().x + boundRectangle[i].width / 2,
//                           boundRectangle[i].tl().y + boundRectangle[i].height / 2);
//            circle(img, center, 10, Scalar(0,255,0), FILLED);

//            以矩形上边中点为画笔
            center = boundRectangle[i].tl();

//            putText(img, objectType, {boundRectangle[i].x, boundRectangle[i].y - 5}, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 1);
        }
    }

    return center;
}

void drawOnImg() {
    for (int i = 0; i < points.size(); ++i) {
        circle(img, points[i], 10, GREEN, FILLED);
    }
}


void checkObject() {

    //角点向量，仅存储轮廓中的角点
    vector<vector<Point>> conPoly(1);

    float peri = arcLength(points, true);
    approxPolyDP(points, conPoly[0], 0.03 * peri, true);
    Rect boundRectangle = boundingRect(conPoly[0]);
    drawContours(img, conPoly, 0, PURPLE, 2);

    //            通过角点数量识别图形
    string objectType;
    int objCornerPoints = (int) conPoly[0].size();
    if (objCornerPoints == 3) {
        vector<vector<Point>> triangle(1);
        minEnclosingTriangle(points, triangle[0]);
        drawContours(img, triangle, 0, RED, 2);
        objectType = "Triangle";
    } else if (objCornerPoints == 4) {
        rectangle(img, boundRectangle, RED, 4);
        float aspectRatio = (float) boundRectangle.width / (float) boundRectangle.height;
        if (aspectRatio > 0.95 && aspectRatio < 1.05) {
            objectType = "Square";
        } else {
            objectType = "Rectangle";
        }
    } else if (objCornerPoints == 5) {
        objectType = "Pentagon";
    } else if (objCornerPoints > 5) {
        objectType = "Circle";
        Point2f center;
        float radius;
        minEnclosingCircle(conPoly[0], center, radius);
        circle(img, center, radius, RED,4);
    } else {
        objectType = "Bad Object";
    }

    putText(img, "Object: " + objectType, {400, 40}, FONT_HERSHEY_PLAIN, 3, ORANGE, 2);
}


int checkOperation() {
    //角点向量，仅存储轮廓中的角点
    vector<vector<Point>> conPoly(1);

    float peri = arcLength(points, true);
    approxPolyDP(points, conPoly[0], 0.03 * peri, true);
    Rect boundRectangle = boundingRect(conPoly[0]);
    drawContours(img, conPoly, 0, PURPLE, 2);

    //            通过角点数量识别图形
    int objectType;
    int objCornerPoints = (int) conPoly[0].size();
    if (objCornerPoints == 3) {
        objectType = 3;
    } else if (objCornerPoints == 4) {
        objectType = 4;
    } else if (objCornerPoints == 5) {
        objectType = 5;
    } else if (objCornerPoints > 5) {
        objectType = 6;
    } else {
        objectType = 7;
    }

    return objectType;
}

int main() {

    VideoCapture capture(0);

    capture.set(CAP_PROP_FRAME_WIDTH, 1280);
    capture.set(CAP_PROP_FRAME_HEIGHT, 720);
    capture.set(CAP_PROP_FPS, 100);//帧率 帧/秒
    capture.set(CAP_PROP_BRIGHTNESS, 61);//亮度
    capture.set(CAP_PROP_CONTRAST, 75);//对比度 40
    capture.set(CAP_PROP_SATURATION, 80);//饱和度 50
    capture.set(CAP_PROP_HUE, 128);//色调 50
    capture.set(CAP_PROP_EXPOSURE, 97);//曝光 50

    cvui::init(window);
    HWND win = GetForegroundWindow();
    HWND win_parent = GetParent(win);

    int windowWidth = 1280;
    int windowHeight = 720;
    int windowX = 240;
    int windowY = 0;

//    namedWindow("Trackbars", (640, 200));
//    createTrackbar("Hue Min", "Trackbars", &hmin, 255);
//    createTrackbar("Hue Max", "Trackbars", &hmax, 255);
//    createTrackbar("Sat Min", "Trackbars", &smin, 255);
//    createTrackbar("Sat Max", "Trackbars", &smax, 255);
//    createTrackbar("Val Min", "Trackbars", &vmin, 255);
//    createTrackbar("Val Max", "Trackbars", &vmax, 255);
//
//    namedWindow("TrackbarsY", (640, 200));
//    createTrackbar("Y Min", "TrackbarsY", &Ymin, 255);
//    createTrackbar("Y Max", "TrackbarsY", &Ymax, 255);
//    createTrackbar("CR Min", "TrackbarsY", &CRmin, 255);
//    createTrackbar("CR Max", "TrackbarsY", &CRmax, 255);
//    createTrackbar("CB Min", "TrackbarsY", &CBmin, 255);
//    createTrackbar("CB Max", "TrackbarsY", &CBmax, 255);

    //通过两种不同的Kernel矩阵进行形态学计算
    kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat kernel_combine = getStructuringElement(MORPH_RECT, Size(4, 4));
    startPoint = Point(-1, -1);

    char key;

    while ((key = (char) waitKey(10)) != 'q') {
        ShowWindow(win, SW_SHOW);
        ShowWindow(win_parent, SW_SHOW);
        resizeWindow(window, windowWidth, windowHeight);
        moveWindow(window, windowX, windowY);

        capture >> img;
        flip(img, img, 1);
        medianBlur(img, imgBlur, 5);

        //用HSV空间识别
        cvtColor(imgBlur, imgHSV, COLOR_BGR2HSV);
        Scalar lower_HSV(hmin, smin, vmin);
        Scalar upper_HSV(hmax, smax, vmax);
        inRange(imgHSV, lower_HSV, upper_HSV, range_HSV);
        erode(range_HSV, range_HSV, kernel);
        dilate(range_HSV, range_HSV, kernel);

        //用YCrCb空间识别
        cvtColor(imgBlur, imgYCrCb, COLOR_BGR2YCrCb);
        Scalar lower_YCrCb(Ymin, CRmin, CBmin);
        Scalar upper_YCrCb(Ymax, CRmax, CBmax);
        inRange(imgYCrCb, lower_YCrCb, upper_YCrCb, range_YCrCb);
        erode(range_YCrCb, range_YCrCb, kernel);
        dilate(range_YCrCb, range_YCrCb, kernel);

        //综合两种识别方式
        bitwise_or(range_HSV, range_YCrCb, img_final);
        medianBlur(img_final, img_final, 3);
        erode(img_final, img_final, kernel_combine);
        dilate(img_final, img_final, kernel_combine);

        Point point = getContours();
//        正常返回点
        if (point.x != -1 && point.y != -1) {
            if (MODE == 1 && !do_operate) {
                let_Mouse_Move(point.x, point.y);
            }

            //          如果没有结束画图
            if (!end_draw && begin_draw) {
//              初次开始画图，设置起点
                if (!start_draw) {
                    startPoint = point;
                    points.push_back(point);
                    start_draw = true;
                }
//             如果在画图过程中回到了起点，则结束画图，清空点集
                else if (((abs(point.x - startPoint.x) < 15) && ((abs(point.y - startPoint.y) < 15))) &&
                         points.size() > 80) {
                    cout << "--------------------" << endl;
                    cout << "End Drawing" << endl;
                    start_draw = false;
                    end_draw = true;
                    begin_draw = false;
                }
                else {
//              将点加入点集
                    points.push_back(point);
                }
            }
        }

        drawOnImg();

        if (end_draw) {
            if (MODE == 0) {
                checkObject();
            }
            else if (MODE == 1) {
                if (!operation_check) {

                    int objectType = checkOperation();
                    string obj, operationType;
                    if (objectType == 3){
                        obj = "Triangle";
                        operationType = "Click";
                    }
                    else if(objectType == 4){
                        obj = "Rectangle";
                        operationType = "DoubleClick";
                    }
                    else if(objectType == 5){
                        obj = "Pentagon";
                        operationType = "MidClick";
                    }
                    else if(objectType == 6){
                        obj = "Circle";
                        operationType = "RightClick";
                    }
                    else{
                        obj = "Bad Object";
                        operationType = "None";
                    }

                    putText(img, "Object: " + obj + "  Operation: " + operationType, {400, 40}, FONT_HERSHEY_PLAIN, 3, ORANGE, 2);
                    imshow(window, img);

                    if(waitKey(2000) < 0){
                        ShowWindow(win, SW_HIDE);
                        ShowWindow(win_parent, SW_HIDE);
                    }

                    if(waitKey(10) < 0){
                        let_Mouse_Move(mousePoint.x, mousePoint.y);
                        if(objectType == 3){
                            let_Mouse_Click();
                        }
                        else if (objectType == 4){
                            let_Mouse_DoubleClick();
                        }
                        else if(objectType == 5){
                            let_Mouse_MidClick();
                        }
                        else if(objectType > 5){
                            let_Mouse_RightClick();
                            cout << "--------------------" << endl;
                            cout << "Exit" << endl;
                            break;
                        }
                    };

                    windowWidth = 576;
                    windowHeight = 324;
                    windowX = 950;
                    windowY = 470;

                    ShowWindow(win, SW_SHOW);
                    ShowWindow(win_parent, SW_SHOW);
                    resizeWindow(window, windowWidth, windowHeight);
                    moveWindow(window, windowX, windowY);

                    begin_draw = false;
                    start_draw = false;
                    end_draw = false;
                    do_operate = false;
                    operation_check = true;
                    points.clear();
                }
            }
        }

        resize(img, img, Size(windowWidth, windowHeight));

        if (cvui::button(img, 0, 0, "Start Drawing") && cvui::mouse(cvui::CLICK)) {
            cout << "--------------------" << endl;
            cout << "Start Drawing" << endl;
            begin_draw = true;
        }

        if (cvui::button(img, 0, 50, "Clear Drawing") && cvui::mouse(cvui::CLICK)) {
            end_draw = false;
            start_draw = false;
            begin_draw = false;
            points.clear();
            cout << "--------------------" << endl;
            cout << "Clear Graph" << endl;
        }

        if (cvui::button(img, 0, 100, "Operating Mode") && cvui::mouse(cvui::CLICK)) {
            MODE = 1;
            end_draw = false;
            start_draw = false;
            do_operate = false;
            points.clear();
            windowWidth = 576;
            windowHeight = 324;
            windowX = 950;
            windowY = 470;
            cout << "--------------------" << endl;
            cout << "Go Operate Mode" << endl;
        }

        if (cvui::button(img, 0, 150, "Save Image") && cvui::mouse(cvui::CLICK)) {
            bool result = imwrite("C:\\Users\\89737\\Desktop\\image.jpg", img);
            cout << "--------------------" << endl;
            cout << "Save Current Image" << endl;
        }

        if ((cvui::button(img, 0, 200, "Restart") && cvui::mouse(cvui::CLICK)) || key == 'r') {
            MODE = 0;
            begin_draw = false;
            start_draw = false;
            end_draw = false;
            do_operate = false;
            operation_check = false;
            startPoint = Point(-1,-1);
            points.clear();
            windowWidth = 1280;
            windowHeight = 720;
            windowX = 200;
            windowY = 0;
            cout << "--------------------" << endl;
            cout << "Restart" << endl;
        }

        if (key == ' ') {
            cout << "--------------------" << endl;
            cout << "start drawing operation..." << endl;
            mousePoint = point;
            points.clear();
            do_operate = true;
            operation_check = false;
            end_draw = false;
            begin_draw = true;
            start_draw = false;
            windowWidth = 1280;
            windowHeight = 720;
            windowX = 200;
            windowY = 0;
        }

        cvui::update();
        imshow(window, img);
    }


    return 0;
}