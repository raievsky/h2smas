cmake_minimum_required(VERSION 3.4)
project(tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CORE_SOURCES_DIR ../core/)

set(SOURCE_FILES tests.cpp
        ${CORE_SOURCES_DIR}/DummyEnvironment.cpp
        ${CORE_SOURCES_DIR}/AgentRequestHandler.cpp)


add_executable(tests ${SOURCE_FILES})

target_include_directories(tests PUBLIC ../core)

find_package(Eigen3 REQUIRED)
target_include_directories(tests PUBLIC ${EIGEN3_INCLUDE_DIR})

target_link_libraries(tests ${Boost_LIBRARIES})
