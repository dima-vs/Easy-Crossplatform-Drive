cmake_minimum_required(VERSION 3.16)

project(ViewLayer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(VIEW_SOURCES
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    logindialog.cpp
    logindialog.h
    logindialog.ui
    propdialog.cpp
    propdialog.h
    propdialog.ui
    signupdialog.cpp
    signupdialog.h
    signupdialog.ui
    res.qrc
)

add_library(View_Lib STATIC ${VIEW_SOURCES})

target_link_libraries(View_Lib PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    # додати ViewModel_Lib
)

add_executable(View_Standalone main.cpp)
target_link_libraries(View_Standalone PRIVATE View_Lib)

target_include_directories(View_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
