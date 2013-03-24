project(cliques)

file(GLOB CLIQUES_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(cliques ${CLIQUES_SOURCE})