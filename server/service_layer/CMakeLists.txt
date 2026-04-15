cmake_minimum_required(VERSION 3.16)
project(ServiceLayer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

set(SERVICE_SOURCES
    auth/service.h auth/service.cpp
    file/service.h file/service.cpp
)
add_library(Service_Lib STATIC ${SERVICE_SOURCES}
    service_result.h
    auth/models.h
    auth/error_codes.h
    email/email_sender.h
    datetime/time_provider_interface.h
    security/password_hasher_interface.h
    auth/auth_config.h
    security/sodium_password_hasher.h security/sodium_password_hasher.cpp
    security/security_config.h
    file/models.h
    file/error_codes.h)
target_link_libraries(Service_Lib PUBLIC Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network PUBLIC DAL_Lib unofficial-sodium::sodium)
target_include_directories(Service_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Service_Standalone main.cpp)

# видалити потім
# target_link_libraries(Service_Standalone PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Service_Standalone PRIVATE Service_Lib)
