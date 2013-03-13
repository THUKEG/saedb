project(pagerank)

file(GLOB PAGERANK_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_executable(pagerank ${PAGERANK_SOURCES})
