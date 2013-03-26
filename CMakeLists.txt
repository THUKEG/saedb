cmake_minimum_required (VERSION 2.8)
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (SubDirList)

project (SAE)

# binary output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SAE_BINARY_DIR}")

# try enable c++11
# g++ and clang++ need flags to enable c++11
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" HAS_CPP11)
if (HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# add sub projects
add_subdirectory (src/saedb)
add_subdirectory (src/app)
