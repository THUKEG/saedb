project(serialization)

set(SERIALIZATION_LIBRARY_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_executable(serialization_test serialization_test.cpp)
target_link_libraries(serialization_test testing)
add_test(NAME serialization_test COMMAND serialization_test)
