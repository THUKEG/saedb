project(inf-max)

file(GLOB LAZYFORWARDINFMAX_SOURCES
    "*.hpp"
    "Heap.cpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "LazyForwardInfMax.hpp"
    "LazyForwardInfMaxTest.cpp"
    )

file(GLOB RANDOMGRAPHINFMAX_SOURCES
    "*.hpp"
    "RandomGraphInfMax.hpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "Heap.cpp"
    "RandomGraphInfMaxTest.cpp"
    )

file(GLOB DEGREEDISCOUNTINFMAX_SOURCES
    "*.hpp"
    "DegreeDiscountInfMax.hpp"
    "mgraph.cpp"
    "graph_builder.cpp"
    "Heap.cpp"
    "DegreeDiscountInfMaxTest.cpp"
    )



add_executable(lazyforwardinfmax-test ${LAZYFORWARDINFMAX_SOURCES})
add_executable(randomgraphinfmax-test ${RANDOMGRAPHINFMAX_SOURCES})
add_executable(degreediscountinfmax-test ${DEGREEDISCOUNTINFMAX_SOURCES})
target_link_libraries(lazyforwardinfmax-test saedb sae_test)
target_link_libraries(randomgraphinfmax-test saedb sae_test)
target_link_libraries(degreediscountinfmax-test saedb sae_test)

add_test(NAME lazyforward COMMAND lazyforwardinfmax-test)
add_test(NAME randomgraph COMMAND randomgraphinfmax-test)
add_test(NAME degreediscount COMMAND degreediscountinfmax-test)
