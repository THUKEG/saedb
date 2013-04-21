cmake_minimum_required(VERSION 2.8)

project (SAE_TEST)

set (SAE_TEST_SOURCES "testharness.cpp" "testharness.hpp")

add_library (sae_test ${SAE_TEST_SOURCES})
