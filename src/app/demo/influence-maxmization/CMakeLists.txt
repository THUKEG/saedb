project(inf-max)

file(GLOB INFMAX_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_executable(inf-max ${INFMAX_SOURCES})
