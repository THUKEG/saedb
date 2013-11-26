cmake_minimum_required (VERSION 2.8)
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (SubDirList)

enable_testing()

# binary output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SAE_BINARY_DIR}")

# try enable c++11
# g++ and clang++ need flags to enable c++11
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" HAS_CPP11)
if (HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# use libc++ on Mac OS X
# by default clang++ will use libstdc++ w/o c++11 support
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif ()

# add includes for all
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")

# add gflag
add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/external/gflags")
# add includes for gflags
include_directories ("${gflags_BINARY_DIR}/include")
# add library path for gflags
set(CMAKE_LIBRARY_PATH "${gflags_BINARY_DIR}/lib" ${CMAKE_LIBRARY_PATH})

# add google-glog
add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/external/google-glog")
# add includes for google-glog
include_directories ("${google-glog_SOURCE_DIR}/src")
# add library path for google-glog
set(CMAKE_LIBRARY_PATH "${google-glog_BINARY_DIR}" ${CMAKE_LIBRARY_PATH})

# add the core libraries
add_subdirectory (testing)
add_subdirectory (indexing)
add_subdirectory (storage)
add_subdirectory (serialization)
add_subdirectory (zrpc)
add_subdirectory (computing)
add_subdirectory (streaming)

# add toolkits
add_all_subdirectories ("${CMAKE_CURRENT_SOURCE_DIR}/toolkit")
