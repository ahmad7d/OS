cmake_minimum_required(VERSION 3.21)
project(resources C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(resources
        demo_itimer.c
        demo_jmp.c
        demo_singInt_handler.c
        uthreads.h uthreads.cpp)
