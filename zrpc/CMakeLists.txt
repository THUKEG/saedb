cmake_minimum_required (VERSION 2.8)
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
include (FindZeroMQ)

project(zrpc)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS rpc.proto)
find_package(Threads REQUIRED)
find_package(ZeroMQ REQUIRED)

include_directories("${PROTOBUF_INCLUDE_DIRS}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${ZEROMQ_INCLUDE_DIRS}")

file(GLOB zrpc_SOURCES
    "*.h"
    "*.hpp"
    "*.cpp"
    )

add_library(zrpc ${zrpc_SOURCES} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(zrpc ${ZEROMQ_LIBRARY} ${PROTOBUF_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
