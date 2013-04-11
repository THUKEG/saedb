project(test_graph)

file(GLOB TEST_GRAPH_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(test_graph ${TEST_GRAPH_SOURCE})
target_link_libraries(test_graph saedb)