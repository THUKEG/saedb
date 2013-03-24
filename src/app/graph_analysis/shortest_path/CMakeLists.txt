project(shortest_path)

file(GLOB SHORTEST_PATH_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(shortest_path ${SHORTEST_PATH_SOURCE})