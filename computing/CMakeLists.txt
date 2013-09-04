project(computing)

file(GLOB COMPUTING_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_library(computing ${COMPUTING_SOURCES})
target_link_libraries(computing mgraph gflags)
