cmake_minimum_required(VERSION 3.24)
project(SharedPtr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("-O2")
add_compile_options("-Wall")
add_compile_options("-Wextra")

add_executable(SharedPtr main.cpp smart_pointers.h)
