project(triangle)

file(GLOB TRIANGLE_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(triangle ${TRIANGLE_SOURCE})