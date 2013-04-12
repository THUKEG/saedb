project(lda)

file(GLOB MAIN_SOURCES
     "*.hpp"
     "*.cpp"
    )
    
add_definitions("-g")
add_executable(lda ${MAIN_SOURCES})
target_link_libraries(lda saedb)
