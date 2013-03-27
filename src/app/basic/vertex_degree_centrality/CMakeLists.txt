project(vertex_degree_centrality)

file (GLOB VDC_SOURCES
	"*.hpp"
	"*.cpp"
	)

add_executable(vertex_degree_centrality ${VDC_SOURCES})
