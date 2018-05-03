cmake_minimum_required(VERSION 3.10)
project(Logger)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/c++)

add_library(logger
        src/c++/Logger.cpp)
