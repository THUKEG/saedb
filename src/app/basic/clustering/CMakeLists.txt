project(clustering)

file(GLOB PAGERANK_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_executable(kmeans ${PAGERANK_SOURCES})
