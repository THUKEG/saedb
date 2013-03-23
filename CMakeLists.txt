cmake_minimum_required (VERSION 2.8)

project (SAE)

# binary output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SAE_BINARY_DIR}")

# enable c++11
add_definitions("-std=c++11")

add_subdirectory (src/saedb)
add_subdirectory (src/app)
