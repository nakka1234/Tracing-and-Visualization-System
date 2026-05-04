cmake_minimum_required(VERSION 3.16)
project(tracing_system)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(shared)
add_subdirectory(tracing_sdk)
add_subdirectory(logging_engine)
add_subdirectory(reconstruction)
add_subdirectory(demo)