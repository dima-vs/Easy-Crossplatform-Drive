cmake_minimum_required(VERSION 3.16)

project(Common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(Common_Lib STATIC 
    dto/auth_dto/register_init_request_dto.h
    dto/auth_dto/register_init_response_dto.h
    dto/auth_dto/confirm_register_request_dto.h
    dto/auth_dto/auth_response_dto.h
    dto/auth_dto/login_request_dto.h
    dto/server_limits_dto/limits_transfer_dto.h
    dto/file_dto/file_tree_response_dto.h
    dto/file_dto/upload_init_request_dto.h
    dto/file_dto/upload_init_response_dto.h
    dto/file_dto/upload_complete_response_dto.h
    dto/file_dto/file_metadata_response_dto.h
    dto/file_dto/file_type_enum.h
    dto/file_dto/file_create_empty_request.h
    dto/file_dto/file_create_empty_response.h
    dto/file_dto/file_rename_request_dto.h
    dto/file_dto/file_rename_response_dto.h

    serialization/auth/auth_json.h
    serialization/auth/auth_json.cpp
    serialization/file/file_json.h
    serialization/file/file_json.cpp
    serialization/server_limits/server_limits_json.h
    serialization/server_limits/server_limits_json.cpp
)
target_link_libraries(Common_Lib PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(Common_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Common_Standalone main.cpp)
target_link_libraries(Common_Standalone PRIVATE Common_Lib Qt${QT_VERSION_MAJOR}::Core)
