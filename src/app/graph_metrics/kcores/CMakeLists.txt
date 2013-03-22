project(kcores)

file(GLOB KCORES_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(kcores ${KCORES_SOURCE})