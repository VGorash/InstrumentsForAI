cmake_minimum_required(VERSION 3.19)
project(ISSII)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_threads simple_threads.c)
add_executable(thread_queue thread_queue.c matrix.c)
add_executable(opencl opencl.c matrix.c)

include_directories("C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v12.1\\include")
target_link_libraries(opencl "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v12.1\\lib\\x64\\OpenCL.lib")
add_custom_command(
        TARGET opencl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/kernel.cl
        ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl)