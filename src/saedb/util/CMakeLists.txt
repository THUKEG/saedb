cmake_minimum_required(VERSION 2.8)
project(thread_pool)

set(THREADPOOL_LIBRARY_SOURCES
    "thread_pool.hpp"
    "thread_pool.cpp"
    )

add_library(thread_pool ${THREADPOOL_LIBRARY_SOURCES})
add_executable(threading_test threadpool_test.cpp)
target_link_libraries(threading_test sae_test)
add_test(NAME threading_test COMMAND threading_test)

