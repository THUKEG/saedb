project(centrality)

file(GLOB CENTRALITY_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(centrality ${CENTRALITY_SOURCE})