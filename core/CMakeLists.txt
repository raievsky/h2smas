cmake_minimum_required(VERSION 3.4)
project(core)

set(BOOST_LIBRARYDIR /usr/lib/x86_64-linux-gnu/)

find_package(Boost REQUIRED COMPONENTS date_time system)
include_directories(${Boost_INCLUDE_DIR})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES HHSMASCore.cpp HHSMASCore.h IncomingConnectionsManager.cpp IncomingConnectionsManager.h TCPConnection.cpp TCPConnection.h EnvironmentManager.cpp EnvironmentManager.h DummyEnvironment.cpp DummyEnvironment.h Position.cpp Position.h AgentRequestHandler.cpp AgentRequestHandler.h EnvironmentIF.cpp EnvironmentIF.h)
add_executable(hhsmas ${SOURCE_FILES})

target_link_libraries(hhsmas ${Boost_LIBRARIES})
