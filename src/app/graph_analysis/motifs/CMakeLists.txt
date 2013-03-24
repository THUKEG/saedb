project(motifs)

file(GLOB MOTIFS_SOURCE
	"*.hpp"
	"*.cpp"
	)

add_executable(motifs ${MOTIFS_SOURCE})