project(serialization)

set(SERIALIZATION_LIBRARY_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_executable(serialization_test serialize_test.cpp)
target_link_libraries(serialization_test sae_test)
add_test(NAME serialization_test COMMAND serialization_test)
