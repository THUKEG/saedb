project(pagerank)

file(GLOB PAGERANK_TEST_SOURCES "*.hpp" "*.cpp")

add_executable(pagerank_test ${PAGERANK_TEST_SOURCES})
target_link_libraries(pagerank_test computing testing)

add_test(NAME pagerank COMMAND pagerank_test)
