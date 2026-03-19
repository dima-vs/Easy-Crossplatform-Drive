cmake_minimum_required(VERSION 3.16)

project(ECD_Server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Sql)

add_subdirectory(data_access_layer)
add_subdirectory(service_layer)
add_subdirectory(api_layer)

add_executable(${PROJECT_NAME}
  main.cpp
)
#target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core Qt6::Network Qt6::Sql)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt${QT_VERSION_MAJOR}::Core
    # API_Lib
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
