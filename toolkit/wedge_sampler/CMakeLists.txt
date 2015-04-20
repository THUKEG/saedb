project(wedge_sampler)

add_executable(wedge_sampler "wedge_sampler.cpp")
target_link_libraries(wedge_sampler ${SGRAPH_BASE})
