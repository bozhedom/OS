cmake_minimum_required(VERSION 3.16)

project(ProcessCounterLogger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(ProcessCounterLogger 
    main.cpp 
    Logger/Logger.cpp 
    Timer/Timer.cpp 
    Counter/Counter.cpp
)

target_include_directories(ProcessCounterLogger PUBLIC 
    Logger 
    Timer 
    Counter
)
