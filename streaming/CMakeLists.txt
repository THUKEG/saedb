project(streaming)

file(GLOB STREAMING_SOURCES "*.hpp" "sgraph.cpp" "sgraph_format_*.cpp")
add_library(streaming ${STREAMING_SOURCES})
target_link_libraries(streaming gflags google-glog)

add_library(sgraph_main "sgraph_main.cpp")
target_link_libraries(sgraph_main streaming)

# NOTE that it's important to include the whole archive.
# Otherwise the graph formats won't get registered.
# TODO add windows specific flags.
# TODO how to disable all_load after sgraph_main?
if (APPLE)
	set(SGRAPH_BASE -Wl,-all_load sgraph_main)
elseif(UNIX)
	set(SGRAPH_BASE -Wl,-whole-archive sgraph_main -Wl,-no-whole-archive)
endif()
# Set the second time to expose to the parent scope
set(SGRAPH_BASE ${SGRAPH_BASE} PARENT_SCOPE)

add_executable(edge_counter "edge_counter.cpp")
target_link_libraries(edge_counter ${SGRAPH_BASE})
