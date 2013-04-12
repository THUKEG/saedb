project(degree_centrality)
file(GLOB DEGREE_SOURCE
	"*.hpp"
	"*.cpp"
	)
add_executable(degree_centrality ${DEGREE_SOURCE})
target_link_libraries(degree_centrality saedb)
