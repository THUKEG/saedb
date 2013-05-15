project(inf-max)

file(GLOB INFMAX_SOURCES
    "*.hpp"
    "Heap.cpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "inf_max.hpp"
    "inf_max_test.cpp"
    )

file(GLOB INFMAX2_SOURCES
    "*.hpp"
    "inf_max2.hpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "Heap.cpp"
    "inf_max2_test.cpp"
    )

file(GLOB INFMAX3_SOURCES
    "*.hpp"
    "inf_max3.hpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "Heap.cpp"
    "inf_max3_test.cpp"
    )



add_executable(inf-max-test ${INFMAX_SOURCES})
add_executable(inf-max2-test ${INFMAX2_SOURCES})
add_executable(inf-max3-test ${INFMAX3_SOURCES})
target_link_libraries(inf-max-test saedb sae_test)
target_link_libraries(inf-max2-test saedb sae_test)
target_link_libraries(inf-max3-test saedb sae_test)

add_test(NAME inf_max COMMAND inf-max-test)
add_test(NAME inf_max2 COMMAND inf-max2-test)
add_test(NAME inf_max3 COMMAND inf-max3-test)
