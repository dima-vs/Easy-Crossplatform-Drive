cmake_minimum_required(VERSION 3.16)
project(ECD_client LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library (Common_Types_Lib STATIC nodata.h ViewConfig.h
    TreeNodeData.h)
target_include_directories(Common_Types_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
