cmake_minimum_required(VERSION 3.4)
project(hhsmas-agents)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp CommFloodingAgent.cpp CommFloodingAgent.h)
add_executable(commFloodingAgent ${SOURCE_FILES})
