cmake_minimum_required(VERSION 2.8)

project(mgraph)

file(GLOB MGRAPH_LIBRARY_SOURCES
    "graph_builder.hpp"
    "graph_builder.cpp"
    "mgraph.hpp"
    "mgraph.cpp"
    "mmap_file.hpp"
    "../graph.hpp"
    )

add_library(mgraph ${MGRAPH_LIBRARY_SOURCES})
add_executable(mgraph_test mgraph_test.cpp)
target_link_libraries(mgraph_test mgraph)
add_executable(mmap_test mmap_file.hpp mmap_test.cpp)
add_executable(type_info_test type_info_test.cpp type_info.cpp)
