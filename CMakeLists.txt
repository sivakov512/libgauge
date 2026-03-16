cmake_minimum_required(VERSION 3.16)

project(gauge C)

add_library(gauge STATIC
    src/gauge.c
)
target_include_directories(gauge PUBLIC include)
