cmake_minimum_required(VERSION 3.21)
project(opencv_project)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(paint virtual.cpp)

set(OpenCV_DIR C:/Users/89737/Downloads/opencv/mingw-build/install)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs opencv_objdetect opencv_video opencv_videoio opencv_features2d)

target_link_libraries(paint ${OpenCV_LIBS})
