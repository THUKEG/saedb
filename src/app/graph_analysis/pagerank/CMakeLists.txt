project(pagerank)

file(GLOB PAGERANK_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(pagerank ${PAGERANK_SOURCE})