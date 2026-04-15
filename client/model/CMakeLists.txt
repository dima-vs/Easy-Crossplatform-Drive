cmake_minimum_required(VERSION 3.16)

project(ModelLayer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# set(MODEL_SOURCES
#
# )

# add_library(Model_Lib STATIC ${MODEL_SOURCES})

# target_link_libraries(Model_Lib PUBLIC Qt${QT_VERSION_MAJOR}::Core)

# target_include_directories(Model_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Model_Standalone main.cpp
    model.h
    model.cpp)

# Видалити після розкоментування бібліотеки вище
target_link_libraries(Model_Standalone PUBLIC Qt${QT_VERSION_MAJOR}::Core Common_Lib)

# target_link_libraries(Model_Standalone PRIVATE Model_Lib)
