cmake_minimum_required(VERSION 3.16)
project(ServiceLayer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# set(SERVICE_SOURCES
#     AuthService.cpp AuthService.h
#     FileService.cpp FileService.h
# )
# add_library(Service_Lib STATIC ${SERVICE_SOURCES})
# target_link_libraries(Service_Lib PUBLIC Qt${QT_VERSION_MAJOR}::Core PRIVATE DAL_Lib)
# target_include_directories(Service_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Service_Standalone main.cpp)

# видалити потім
target_link_libraries(Service_Standalone PUBLIC Qt${QT_VERSION_MAJOR}::Core)
# target_link_libraries(Service_Standalone PRIVATE Service_Lib)
