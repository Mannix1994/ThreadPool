cmake_minimum_required(VERSION 3.12)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 11)

add_executable(ThreadPool main.cpp ThreadPool/ThreadPool.h ThreadPool/Semaphore.h ThreadPool/timer_std.h)

target_link_libraries(${PROJECT_NAME} pthread)