cmake_minimum_required (VERSION 2.8)

project (indexing)

file (GLOB INDEXING_SOURCES
    "analyzer.hpp"
    "indexing.hpp"
    "indexing.cpp"
    "query.hpp"
    "query.cpp"
    "search.hpp"
    )

add_library(indexing ${INDEXING_SOURCES})
target_link_libraries(indexing ArnetAnalyzer)

add_executable(indexing_test indexing_test.cpp)
target_link_libraries(indexing_test indexing testing)
add_test(NAME indexing_test COMMAND indexing_test)

add_subdirectory(analyzer)
