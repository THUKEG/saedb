project(saedb)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(io)
add_subdirectory(test)
add_subdirectory(util)

file(GLOB SAE_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_library(saedb ${SAE_SOURCES})
target_link_libraries(saedb mgraph)
