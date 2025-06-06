cmake_minimum_required(VERSION 3.13)

project(ZeroMqIpc
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ZMQROOT ${CMAKE_CURRENT_LIST_DIR}/zmq)

set(IPCROOT ${CMAKE_CURRENT_LIST_DIR}/ipc)

add_executable(ZmqIpcServer)

add_executable(ZmqIpcClient)

message(STATUS "add zmq root sub directory")

add_subdirectory(${ZMQROOT})

message(STATUS "add ipc root directory")

add_subdirectory(${IPCROOT})
