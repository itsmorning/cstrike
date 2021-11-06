cmake_minimum_required(VERSION 3.20.5)
project(cstrike)

file(GLOB_RECURSE SRC "src/*.cpp")

set(CMAKE_CXX_STANDARD 20)

add_library(cstrike SHARED ${SRC})
