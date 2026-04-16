cmake_minimum_required(VERSION 3.13)

project(ECD_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

add_subdirectory(server)

add_executable(unit_tests
    server_test.cpp
    ${SERVER_TEST_SOURCES}
)

target_link_libraries(unit_tests
    PRIVATE
        GTest::gtest
        GTest::gmock
        # GTest::gtest_main
        Service_Lib
        DAL_Lib
        Qt6::Core
)

include(GoogleTest)
gtest_add_tests(TARGET unit_tests)
