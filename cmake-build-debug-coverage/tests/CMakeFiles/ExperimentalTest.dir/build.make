# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/df/Documents/install/CLion-2021.2.3/clion-2021.2.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/df/Documents/install/CLion-2021.2.3/clion-2021.2.3/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/df/Documents/workspace/async_runtime

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage

# Utility rule file for ExperimentalTest.

# Include any custom commands dependencies for this target.
include tests/CMakeFiles/ExperimentalTest.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/ExperimentalTest.dir/progress.make

tests/CMakeFiles/ExperimentalTest:
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/tests && /home/df/Documents/install/CLion-2021.2.3/clion-2021.2.3/bin/cmake/linux/bin/ctest -D ExperimentalTest

ExperimentalTest: tests/CMakeFiles/ExperimentalTest
ExperimentalTest: tests/CMakeFiles/ExperimentalTest.dir/build.make
.PHONY : ExperimentalTest

# Rule to build all files generated by this target.
tests/CMakeFiles/ExperimentalTest.dir/build: ExperimentalTest
.PHONY : tests/CMakeFiles/ExperimentalTest.dir/build

tests/CMakeFiles/ExperimentalTest.dir/clean:
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/tests && $(CMAKE_COMMAND) -P CMakeFiles/ExperimentalTest.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/ExperimentalTest.dir/clean

tests/CMakeFiles/ExperimentalTest.dir/depend:
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/df/Documents/workspace/async_runtime /home/df/Documents/workspace/async_runtime/tests /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/tests /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/tests/CMakeFiles/ExperimentalTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/ExperimentalTest.dir/depend

