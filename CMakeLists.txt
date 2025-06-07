cmake_minimum_required(VERSION 3.13)

project(ZeroMqIpc
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ZMQROOT ${CMAKE_CURRENT_LIST_DIR}/zmq)

set(IPCROOT ${CMAKE_CURRENT_LIST_DIR}/ipc)

set(PROTOBUFROOT ${CMAKE_CURRENT_LIST_DIR}/protobuf)

#thrid part zmq as a interface lib.
add_library(ZeroMqInterface INTERFACE)

#thrid part protobuf as a interface lib.
add_library(ProtobufInface INTERFACE)

add_executable(ZmqIpcServer)

add_executable(ZmqIpcClient)

message(STATUS "add zmq root sub directory")

add_subdirectory(${ZMQROOT})

message(STATUS "add protobuf root directory")

add_subdirectory(${PROTOBUFROOT})

message(STATUS "add ipc root directory")

add_subdirectory(${IPCROOT})
