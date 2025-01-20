cmake_minimum_required(VERSION 3.16)

project(EnvTempMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Поиск библиотеки потоков
find_package(Threads REQUIRED)

# Указываем исполнимый файл и исходники
add_executable(EnvTempMonitor main.cpp)

# Связываем с библиотекой потоков
target_link_libraries(EnvTempMonitor PRIVATE Threads::Threads)
