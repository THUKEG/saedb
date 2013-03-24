project(community)

file(GLOB COMMUNITY_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(community ${COMMUNITY_SOURCE})