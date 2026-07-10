cmake_minimum_required(VERSION 3.15)
project(FastDataParser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/02 /arch:AVX2 /W4)
else()
    add_compile_options(-03 -march=native -Wall -Wextra - pthread)
endif()

add_executable(${PROJECT_NAME} app/src/main.cpp)