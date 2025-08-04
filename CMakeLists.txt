cmake_minimum_required(VERSION 3.13)

project(ZeroMqIpc
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IPCROOT ${CMAKE_CURRENT_LIST_DIR}/rpc)

add_library(ZmqIpcServer STATIC)

message(STATUS "add zmq root sub directory")

message(STATUS "add protobuf root directory")

message(STATUS "add ipc root directory")

target_include_directories(ZmqIpcServer PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# find_library(log-lib )

target_link_libraries(ZmqIpcServer PRIVATE log)

target_sources(ZmqIpcServer PUBLIC 
    # src/ZUdsRpcServer.cpp
    src/NngUdsRpcPeerListener.cpp
    src/NngUdsRpcPeerDial.cpp
)

# target_link_libraries(ZmqIpcServer)

install(TARGETS ZmqIpcServer
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)