cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" 
        CACHE FILEPATH "vcpkg toolchain file")
endif()

project(EasyCrossplatformDrive)

find_package(unofficial-sodium REQUIRED)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

# enable_testing()
# add_subdirectory(tests)
