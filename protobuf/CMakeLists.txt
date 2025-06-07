cmake_minimum_required(VERSION 3.13)

message(STATUS "start protobuf add sub directory")

set(ProtoBufInclude ${CMAKE_CURRENT_LIST_DIR}/include)

file(GLOB ProtoBufLiteLib "${CMAKE_CURRENT_LIST_DIR}/lib/*.a")

# set(ProtoBufLiteLib 
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libprotobuf-lite.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_entry.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_flags.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_globals.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_initialize.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_check_op.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_conditions.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_fnmatch.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_format.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_globals.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_log_sink_set.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_message.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_nullguard.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_proto.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_internal_structured_proto.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_severity.a
#     ${CMAKE_CURRENT_LIST_DIR}/lib/libabsl_log_sink.a
# )

target_include_directories(ProtobufInface INTERFACE ${ProtoBufInclude})

target_link_libraries(ProtobufInface INTERFACE ${ProtoBufLiteLib}  log)

message(STATUS "end protobuf add sub directory")