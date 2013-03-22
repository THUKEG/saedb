project(cascades)

file(GLOB CASCADES_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(cascades ${CASCADES_SOURCE})