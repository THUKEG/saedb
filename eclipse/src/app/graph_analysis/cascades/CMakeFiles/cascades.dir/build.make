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
CMAKE_SOURCE_DIR = /home/neo/Projects/saedb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/neo/Projects/saedb/eclipse

# Include any dependencies generated for this target.
include src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/depend.make

# Include the progress variables for this target.
include src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/progress.make

# Include the compile flags for this target's objects.
include src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/flags.make

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o: src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/flags.make
src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o: ../src/app/graph_analysis/cascades/cascades.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/neo/Projects/saedb/eclipse/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o"
	cd /home/neo/Projects/saedb/eclipse/src/app/graph_analysis/cascades && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/cascades.dir/cascades.cpp.o -c /home/neo/Projects/saedb/src/app/graph_analysis/cascades/cascades.cpp

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cascades.dir/cascades.cpp.i"
	cd /home/neo/Projects/saedb/eclipse/src/app/graph_analysis/cascades && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/neo/Projects/saedb/src/app/graph_analysis/cascades/cascades.cpp > CMakeFiles/cascades.dir/cascades.cpp.i

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cascades.dir/cascades.cpp.s"
	cd /home/neo/Projects/saedb/eclipse/src/app/graph_analysis/cascades && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/neo/Projects/saedb/src/app/graph_analysis/cascades/cascades.cpp -o CMakeFiles/cascades.dir/cascades.cpp.s

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.requires:
.PHONY : src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.requires

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.provides: src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.requires
	$(MAKE) -f src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/build.make src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.provides.build
.PHONY : src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.provides

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.provides.build: src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o

# Object files for target cascades
cascades_OBJECTS = \
"CMakeFiles/cascades.dir/cascades.cpp.o"

# External object files for target cascades
cascades_EXTERNAL_OBJECTS =

cascades: src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o
cascades: src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/build.make
cascades: src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../../../cascades"
	cd /home/neo/Projects/saedb/eclipse/src/app/graph_analysis/cascades && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cascades.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/build: cascades
.PHONY : src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/build

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/requires: src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/cascades.cpp.o.requires
.PHONY : src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/requires

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/clean:
	cd /home/neo/Projects/saedb/eclipse/src/app/graph_analysis/cascades && $(CMAKE_COMMAND) -P CMakeFiles/cascades.dir/cmake_clean.cmake
.PHONY : src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/clean

src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/depend:
	cd /home/neo/Projects/saedb/eclipse && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/neo/Projects/saedb /home/neo/Projects/saedb/src/app/graph_analysis/cascades /home/neo/Projects/saedb/eclipse /home/neo/Projects/saedb/eclipse/src/app/graph_analysis/cascades /home/neo/Projects/saedb/eclipse/src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/app/graph_analysis/cascades/CMakeFiles/cascades.dir/depend

