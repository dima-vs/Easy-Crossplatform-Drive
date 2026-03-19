cmake_minimum_required(VERSION 3.16)
project(DataAccessLayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

# set(DAL_SOURCES
    # DatabaseManager.cpp, UserRepository.cpp
    # DatabaseManager.h, UserRepository.h
# )

# add_library(DAL_Lib STATIC ${DAL_SOURCES})

# target_link_libraries(DAL_Lib PUBLIC Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Core)
#target_include_directories(DAL_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(DAL_Standalone main.cpp)
# видалити потім
target_link_libraries(DAL_Standalone PUBLIC Qt${QT_VERSION_MAJOR}::Core)
#target_link_libraries(DAL_Standalone PRIVATE DAL_Lib)