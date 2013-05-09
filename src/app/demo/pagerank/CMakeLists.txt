project(pagerank)

file(GLOB PAGERANK_SOURCES "pagerank.*")
file(GLOB PAGERANK_TEST_SOURCES "pagerank_test.*")

add_library(pagerank ${PAGERANK_SOURCES})
target_link_libraries(pagerank saedb)

add_executable(pagerank_test ${PAGERANK_TEST_SOURCES})
target_link_libraries(pagerank_test pagerank sae_test)

add_test(NAME pagerank COMMAND pagerank_test)
