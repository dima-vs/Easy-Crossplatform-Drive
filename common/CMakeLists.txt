cmake_minimum_required(VERSION 3.16)

project(Common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(Domain_Lib STATIC
    domain/file_type.h
    converter/file_type_converter.h
)

target_link_libraries(Domain_Lib PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(Domain_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Common_Lib STATIC
    dto/auth/register_init_request.h
    dto/auth/register_init_response.h
    dto/auth/confirm_register_request.h
    dto/auth/general_response.h
    dto/auth/login_request.h
    dto/server_limits/transfer.h

    dto/file/tree_response.h
    dto/file/upload_init_request.h
    dto/file/upload_init_response.h
    dto/file/upload_complete_response.h
    dto/file/metadata_response.h
    dto/file/create_empty_request.h
    dto/file/create_empty_response.h
    dto/file/rename_request.h
    dto/file/rename_response.h

    serialization/auth/json.h
    serialization/auth/json.cpp
    serialization/file/json.h
    serialization/file/json.cpp
    serialization/server_limits/json.h
    serialization/server_limits/json.cpp
)
target_link_libraries(Common_Lib PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(Common_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Common_Standalone main.cpp)
target_link_libraries(Common_Standalone PRIVATE Common_Lib Qt${QT_VERSION_MAJOR}::Core)
