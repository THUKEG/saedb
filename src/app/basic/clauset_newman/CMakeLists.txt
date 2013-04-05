project(clauset_newman)
file(GLOB CLAUSET_NEWMAN_SOURCE
	"*.hpp"
	"*.cpp"
	)
add_executable(clauset_newman ${CLAUSET_NEWMAN_SOURCE})
