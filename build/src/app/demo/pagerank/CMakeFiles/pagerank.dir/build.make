# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/neo/projects/saedb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/neo/projects/saedb/build

# Include any dependencies generated for this target.
include src/app/demo/pagerank/CMakeFiles/pagerank.dir/depend.make

# Include the progress variables for this target.
include src/app/demo/pagerank/CMakeFiles/pagerank.dir/progress.make

# Include the compile flags for this target's objects.
include src/app/demo/pagerank/CMakeFiles/pagerank.dir/flags.make

src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o: src/app/demo/pagerank/CMakeFiles/pagerank.dir/flags.make
src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o: ../src/app/demo/pagerank/pagerank.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/neo/projects/saedb/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o"
	cd /home/neo/projects/saedb/build/src/app/demo/pagerank && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/pagerank.dir/pagerank.cpp.o -c /home/neo/projects/saedb/src/app/demo/pagerank/pagerank.cpp

src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pagerank.dir/pagerank.cpp.i"
	cd /home/neo/projects/saedb/build/src/app/demo/pagerank && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/neo/projects/saedb/src/app/demo/pagerank/pagerank.cpp > CMakeFiles/pagerank.dir/pagerank.cpp.i

src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pagerank.dir/pagerank.cpp.s"
	cd /home/neo/projects/saedb/build/src/app/demo/pagerank && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/neo/projects/saedb/src/app/demo/pagerank/pagerank.cpp -o CMakeFiles/pagerank.dir/pagerank.cpp.s

src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.requires:
.PHONY : src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.requires

src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.provides: src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.requires
	$(MAKE) -f src/app/demo/pagerank/CMakeFiles/pagerank.dir/build.make src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.provides.build
.PHONY : src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.provides

src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.provides.build: src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o

# Object files for target pagerank
pagerank_OBJECTS = \
"CMakeFiles/pagerank.dir/pagerank.cpp.o"

# External object files for target pagerank
pagerank_EXTERNAL_OBJECTS =

pagerank: src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o
pagerank: src/app/demo/pagerank/CMakeFiles/pagerank.dir/build.make
pagerank: src/app/demo/pagerank/CMakeFiles/pagerank.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../../../pagerank"
	cd /home/neo/projects/saedb/build/src/app/demo/pagerank && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pagerank.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/app/demo/pagerank/CMakeFiles/pagerank.dir/build: pagerank
.PHONY : src/app/demo/pagerank/CMakeFiles/pagerank.dir/build

src/app/demo/pagerank/CMakeFiles/pagerank.dir/requires: src/app/demo/pagerank/CMakeFiles/pagerank.dir/pagerank.cpp.o.requires
.PHONY : src/app/demo/pagerank/CMakeFiles/pagerank.dir/requires

src/app/demo/pagerank/CMakeFiles/pagerank.dir/clean:
	cd /home/neo/projects/saedb/build/src/app/demo/pagerank && $(CMAKE_COMMAND) -P CMakeFiles/pagerank.dir/cmake_clean.cmake
.PHONY : src/app/demo/pagerank/CMakeFiles/pagerank.dir/clean

src/app/demo/pagerank/CMakeFiles/pagerank.dir/depend:
	cd /home/neo/projects/saedb/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/neo/projects/saedb /home/neo/projects/saedb/src/app/demo/pagerank /home/neo/projects/saedb/build /home/neo/projects/saedb/build/src/app/demo/pagerank /home/neo/projects/saedb/build/src/app/demo/pagerank/CMakeFiles/pagerank.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/app/demo/pagerank/CMakeFiles/pagerank.dir/depend

