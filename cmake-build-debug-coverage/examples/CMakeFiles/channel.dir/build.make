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

# Include any dependencies generated for this target.
include examples/CMakeFiles/channel.dir/depend.make
# Include the progress variables for this target.
include examples/CMakeFiles/channel.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/channel.dir/flags.make

examples/CMakeFiles/channel.dir/channel.cpp.o: examples/CMakeFiles/channel.dir/flags.make
examples/CMakeFiles/channel.dir/channel.cpp.o: ../examples/channel.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/channel.dir/channel.cpp.o"
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/channel.dir/channel.cpp.o -c /home/df/Documents/workspace/async_runtime/examples/channel.cpp

examples/CMakeFiles/channel.dir/channel.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/channel.dir/channel.cpp.i"
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/df/Documents/workspace/async_runtime/examples/channel.cpp > CMakeFiles/channel.dir/channel.cpp.i

examples/CMakeFiles/channel.dir/channel.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/channel.dir/channel.cpp.s"
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/df/Documents/workspace/async_runtime/examples/channel.cpp -o CMakeFiles/channel.dir/channel.cpp.s

# Object files for target channel
channel_OBJECTS = \
"CMakeFiles/channel.dir/channel.cpp.o"

# External object files for target channel
channel_EXTERNAL_OBJECTS =

examples/channel: examples/CMakeFiles/channel.dir/channel.cpp.o
examples/channel: examples/CMakeFiles/channel.dir/build.make
examples/channel: libar.so
examples/channel: /usr/lib64/libboost_thread.so.1.72.0
examples/channel: /usr/lib64/libboost_chrono.so.1.72.0
examples/channel: /usr/lib64/libboost_system.so.1.72.0
examples/channel: /usr/lib64/libboost_regex.so.1.72.0
examples/channel: /usr/lib64/libboost_context.so.1.72.0
examples/channel: /usr/lib64/libuv.so
examples/channel: examples/CMakeFiles/channel.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable channel"
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/channel.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/channel.dir/build: examples/channel
.PHONY : examples/CMakeFiles/channel.dir/build

examples/CMakeFiles/channel.dir/clean:
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/examples && $(CMAKE_COMMAND) -P CMakeFiles/channel.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/channel.dir/clean

examples/CMakeFiles/channel.dir/depend:
	cd /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/df/Documents/workspace/async_runtime /home/df/Documents/workspace/async_runtime/examples /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/examples /home/df/Documents/workspace/async_runtime/cmake-build-debug-coverage/examples/CMakeFiles/channel.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/channel.dir/depend

