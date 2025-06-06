cmake_minimum_required(VERSION 3.13)

message(STATUS "start ipc add sub directory")

set(public_header 
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcServer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcClient.hpp
)

target_include_directories(
    ZmqIpcServer PUBLIC ${PUBLIC}
)

target_include_directories(
    ZmqIpcClient PUBLIC ${PUBLIC}
)

target_sources(
    ZmqIpcServer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcServer.cpp
)

target_sources(
    ZmqIpcClient PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcClient.cpp
)

message(STATUS "end ipc add sub directory")