cmake_minimum_required(VERSION 3.13)

message(STATUS "start zmq add sub directory")

set(ZeroMqInclude ${CMAKE_CURRENT_LIST_DIR}/include)

set_property(TARGET ZmqIpcServer PROPERTY INCLUDE_DIRECTORIES ${ZeroMqInclude})

set_property(TARGET ZmqIpcClient PROPERTY INCLUDE_DIRECTORIES ${ZeroMqInclude})

set(ZeroMqLib ${CMAKE_CURRENT_LIST_DIR}/lib/libzmq.a)

set(public_header 
    ${ZeroMqInclude}/zmq.h
    ${ZeroMqInclude}/zmq_utils.h
)

# add_library(libzmq STATIC IMPORTED)

# set_target_properties(libzmq PROPERTIES
#     IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/libzmq.a
# )

target_include_directories(
    ZmqIpcServer PUBLIC ${ZeroMqInclude}
)

target_include_directories(
    ZmqIpcClient PUBLIC ${ZeroMqInclude}
)

target_link_libraries(ZmqIpcServer PUBLIC
    ${ZeroMqLib}
)

target_link_libraries(ZmqIpcClient PUBLIC
    ${ZeroMqLib}
)

message(STATUS "end zmq add sub directory")