cmake_minimum_required(VERSION 3.19)
project(ISSII)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_threads simple_threads.c)
add_executable(thread_queue thread_queue.c matrix.c)