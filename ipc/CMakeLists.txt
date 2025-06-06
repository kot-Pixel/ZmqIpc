cmake_minimum_required(VERSION 3.13)

message(STATUS "start ipc add sub directory")

set(public_header ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcServer.hpp)

target_include_directories(
    ZmqIpc PUBLIC ${PUBLIC}
)

target_sources(
    ZmqIpc PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcServer.cpp
)

message(STATUS "end ipc add sub directory")