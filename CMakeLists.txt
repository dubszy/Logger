cmake_minimum_required(VERSION 3.6)
project(Logger)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_library(logger
        src/c++/Logger.cpp)
