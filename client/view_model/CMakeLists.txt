cmake_minimum_required(VERSION 3.16)

project(ViewModelLayer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)


#set(VIEWMODEL_SOURCES
    # Додавати .cpp, .h (окрім main.cpp)
#)

#add_library(ViewModel_Lib STATIC ${VIEWMODEL_SOURCES})

#target_link_libraries(ViewModel_Lib 
#    PUBLIC Qt${QT_VERSION_MAJOR}::Core
#    PRIVATE Model_Lib
#)

add_executable(ViewModel_Standalone main.cpp)

# видалити після додавання бібліотеки
target_link_libraries(ViewModel_Standalone PUBLIC Qt${QT_VERSION_MAJOR}::Core)

#target_link_libraries(ViewModel_Standalone PRIVATE ViewModel_Lib)
