cmake_minimum_required(VERSION 3.12)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 11)

add_executable(ThreadPool main.cpp Semaphore.h ThreadPool.h timer.h)

target_link_libraries(${PROJECT_NAME} pthread)