include_directories ("${SAE_SOURCE_DIR}/src/saedb")

subdir_list (CATEGORIES ${CMAKE_CURRENT_SOURCE_DIR})

#foreach (CATEGORY ${CATEGORIES})
#	add_all_subdirectories (${CATEGORY})
#endforeach()
add_subdirectory("demo")
#add_subdirectory("basic")
