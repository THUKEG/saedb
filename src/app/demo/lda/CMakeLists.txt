project(lda)

file(GLOB MAIN_SOURCES
	"graph_builder.hpp"
	"mgraph.hpp"
    "lda.cpp"
    )
    
add_definitions("-g")
add_executable(lda ${MAIN_SOURCES})

