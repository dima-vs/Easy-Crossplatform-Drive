cmake_minimum_required(VERSION 3.16)
project(APILayer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core HttpServer)

set(API_SOURCES
    auth/auth_controller.h
    auth/auth_controller.cpp
    file/file_controller.h
    file/file_controller.cpp
    file/file_models_converter.h
    file/file_models_converter.cpp
)
add_library(API_Lib STATIC ${API_SOURCES})
target_link_libraries(API_Lib PUBLIC Qt${QT_VERSION_MAJOR}::HttpServer PUBLIC Service_Lib)
target_include_directories(API_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(API_Lib PUBLIC Common_Lib)

add_executable(API_Standalone main.cpp)

# видалити потім
# target_link_libraries(API_Standalone PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::HttpServer)
target_link_libraries(API_Standalone PRIVATE API_Lib)
