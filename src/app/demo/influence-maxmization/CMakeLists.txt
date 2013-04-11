project(inf-max)

file(GLOB INFMAX_SOURCES
    "*.hpp"
    "Heap.cpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "inf_max.cpp"
    )

file(GLOB INFMAX2_SOURCES
    "*.hpp"
    "inf_max2.cpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "Heap.cpp"
    )

file(GLOB INFMAX3_SOURCES
    "*.hpp"
    "inf_max3.cpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "Heap.cpp"
    )



add_executable(inf-max ${INFMAX_SOURCES})
add_executable(inf-max2 ${INFMAX2_SOURCES})
add_executable(inf-max3 ${INFMAX3_SOURCES})
target_link_libraries(inf-max saedb)
target_link_libraries(inf-max2 saedb)
target_link_libraries(inf-max3 saedb)
