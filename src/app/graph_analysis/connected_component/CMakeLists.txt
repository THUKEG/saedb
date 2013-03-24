project(connected_component)

file(GLOB CONNECTED_COMPONENT_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(connected_component ${CONNECTED_COMPONENT_SOURCE})