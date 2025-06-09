cmake_minimum_required(VERSION 3.13)

message(STATUS "start ipc add sub directory")


get_target_property(PB_INCLUDE_DIRS ProtobufInface INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "protobuf interface include dirs: ${PB_INCLUDE_DIRS}")

#find_library(ZeroMqInterface)

#message(STATUS "zeroMq interface found ${ZeroMqInterface_FOUND}")

set(public_header
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcDefine.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcServer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcWorker.hpp

    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcClient.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcUtils.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcDispatcher.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSoctetIpcHandler.hpp

    ${CMAKE_CURRENT_LIST_DIR}/ipc.pb.h

    ${CMAKE_CURRENT_LIST_DIR}/message.pb.h
)

target_link_libraries(
    ZmqIpcServer PRIVATE ZeroMqInterface ProtobufInface
)

target_link_libraries(
    ZmqIpcClient PRIVATE ZeroMqInterface ProtobufInface
)

target_sources(
    ZmqIpcServer PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcWorker.cpp

    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcUtils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcDispatcher.cpp

    ${CMAKE_CURRENT_LIST_DIR}/ipc.pb.cc

    ${CMAKE_CURRENT_LIST_DIR}/message.pb.cc
)

target_sources(
    ZmqIpcClient PUBLIC 
    
    ${CMAKE_CURRENT_LIST_DIR}/ZSocketIpcClient.cpp
    ${CMAKE_CURRENT_LIST_DIR}/message.pb.cc

    ${CMAKE_CURRENT_LIST_DIR}/ipc.pb.cc
)

message(STATUS "end ipc add sub directory")