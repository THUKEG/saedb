project(LDA)

file(GLOB LDA_TEST_SOURCES "*.hpp" "*.cpp")

add_executable(lda_test ${LDA_TEST_SOURCES})
target_link_libraries(lda_test computing)

