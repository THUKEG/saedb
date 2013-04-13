project(shortest_path)

file (GLOB SP_SOURCES
	"*.hpp"
	"*.cpp"
	)

add_executable(shortest_path ${SP_SOURCES})
target_link_libraries(shortest_path saedb)
