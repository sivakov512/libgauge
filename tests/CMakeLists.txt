cmake_minimum_required(VERSION 3.16)

set(LIBNAME gauge)

project(gauge_tests C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup Unity
include(FetchContent)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.1
)
FetchContent_MakeAvailable(Unity)

if(NOT TARGET unity)
    add_library(unity STATIC ${Unity_SOURCE_DIR}/src/unity.c)
    target_include_directories(unity PUBLIC ${Unity_SOURCE_DIR}/src)
endif()

enable_testing()

# Add project
add_subdirectory(
    ${CMAKE_CURRENT_LIST_DIR}/../ ${CMAKE_BINARY_DIR}/${LIBNAME})

# Test utilities
add_subdirectory(tu)
add_subdirectory(test_tu)

# Tests
add_subdirectory(test_cv)
add_subdirectory(test_gauge)
add_subdirectory(test_utils)
