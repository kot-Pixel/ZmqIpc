cmake_minimum_required(VERSION 3.13)

message(STATUS "start zmq add sub directory")

set(ZeroMqInclude ${CMAKE_CURRENT_LIST_DIR}/include)

set(ZeroMqStaticLib ${CMAKE_CURRENT_LIST_DIR}/lib/libzmq.a)

target_include_directories(ZeroMqInterface INTERFACE ${ZeroMqInclude})

target_link_libraries(ZeroMqInterface INTERFACE ${ZeroMqStaticLib})

message(STATUS "end zmq add sub directory")