cmake_minimum_required(VERSION 3.13)

message(STATUS "start zmq add sub directory")

set(ZeroMqInclude ${CMAKE_CURRENT_LIST_DIR}/include)

set_property(TARGET ZmqIpc PROPERTY INCLUDE_DIRECTORIES ${ZeroMqInclude})

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
    ZmqIpc PUBLIC ${ZeroMqInclude}
)

target_link_libraries(ZmqIpc PUBLIC
    ${ZeroMqLib}

)

message(STATUS "zmq ipc .a found ${ZmqIpc_FOUND}")

message(STATUS "end zmq add sub directory")