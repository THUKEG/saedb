project(saedb)

file(GLOB SAE_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_library(saedb ${SAE_SOURCES})
