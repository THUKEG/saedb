project(utils)

file(GLOB UTILS_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(utils ${UTILS_SOURCE})