# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/cameron/Documents/cppTutorials/R18Z4Sim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cameron/Documents/cppTutorials/R18Z4Sim

# Include any dependencies generated for this target.
include CMakeFiles/R18Z4Sim.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/R18Z4Sim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/R18Z4Sim.dir/flags.make

CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.o: CMakeFiles/R18Z4Sim.dir/flags.make
CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.o: R18Z4Sim.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cameron/Documents/cppTutorials/R18Z4Sim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.o -c /home/cameron/Documents/cppTutorials/R18Z4Sim/R18Z4Sim.cpp

CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cameron/Documents/cppTutorials/R18Z4Sim/R18Z4Sim.cpp > CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.i

CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cameron/Documents/cppTutorials/R18Z4Sim/R18Z4Sim.cpp -o CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.s

# Object files for target R18Z4Sim
R18Z4Sim_OBJECTS = \
"CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.o"

# External object files for target R18Z4Sim
R18Z4Sim_EXTERNAL_OBJECTS =

R18Z4Sim: CMakeFiles/R18Z4Sim.dir/R18Z4Sim.cpp.o
R18Z4Sim: CMakeFiles/R18Z4Sim.dir/build.make
R18Z4Sim: CMakeFiles/R18Z4Sim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cameron/Documents/cppTutorials/R18Z4Sim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable R18Z4Sim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/R18Z4Sim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/R18Z4Sim.dir/build: R18Z4Sim

.PHONY : CMakeFiles/R18Z4Sim.dir/build

CMakeFiles/R18Z4Sim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/R18Z4Sim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/R18Z4Sim.dir/clean

CMakeFiles/R18Z4Sim.dir/depend:
	cd /home/cameron/Documents/cppTutorials/R18Z4Sim && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cameron/Documents/cppTutorials/R18Z4Sim /home/cameron/Documents/cppTutorials/R18Z4Sim /home/cameron/Documents/cppTutorials/R18Z4Sim /home/cameron/Documents/cppTutorials/R18Z4Sim /home/cameron/Documents/cppTutorials/R18Z4Sim/CMakeFiles/R18Z4Sim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/R18Z4Sim.dir/depend

