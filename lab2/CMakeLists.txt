cmake_minimum_required(VERSION 3.16)

project(BackgroundProcess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(BackgroundProcess lib/BackgroundProcess.cpp)

add_executable(TestUtility TestUtility.cpp)

target_include_directories(BackgroundProcess PUBLIC lib)

target_link_libraries(TestUtility BackgroundProcess)
