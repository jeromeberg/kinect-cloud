cmake_minimum_required(VERSION 3.16)
project(kinect-cloud LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FREENECT REQUIRED IMPORTED_TARGET libfreenect)

find_library(FREENECT_SYNC_LIBRARY
    NAMES freenect_sync
    HINTS ${FREENECT_LIBRARY_DIRS})
if(NOT FREENECT_SYNC_LIBRARY)
    message(FATAL_ERROR "libfreenect_sync not found")
endif()

add_library(kc
    src/cloud.cpp
    src/device.cpp
    src/log.cpp
    src/recorder.cpp
    src/replay.cpp)

target_include_directories(kc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(kc PRIVATE PkgConfig::FREENECT ${FREENECT_SYNC_LIBRARY})
target_compile_options(kc PRIVATE -Wall -Wextra -Werror)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(apps/kc-view)
    add_subdirectory(apps/kc-record)
endif()
