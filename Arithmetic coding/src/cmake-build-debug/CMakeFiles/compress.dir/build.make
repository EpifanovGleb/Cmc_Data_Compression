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
CMAKE_COMMAND = /snap/clion/169/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/169/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gleb/compression/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gleb/compression/src/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/compress.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/compress.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/compress.dir/flags.make

CMakeFiles/compress.dir/main.c.o: CMakeFiles/compress.dir/flags.make
CMakeFiles/compress.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gleb/compression/src/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/compress.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/compress.dir/main.c.o -c /home/gleb/compression/src/main.c

CMakeFiles/compress.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/compress.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gleb/compression/src/main.c > CMakeFiles/compress.dir/main.c.i

CMakeFiles/compress.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/compress.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gleb/compression/src/main.c -o CMakeFiles/compress.dir/main.c.s

# Object files for target compress
compress_OBJECTS = \
"CMakeFiles/compress.dir/main.c.o"

# External object files for target compress
compress_EXTERNAL_OBJECTS =

compress: CMakeFiles/compress.dir/main.c.o
compress: CMakeFiles/compress.dir/build.make
compress: libutils.a
compress: CMakeFiles/compress.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gleb/compression/src/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable compress"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/compress.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/compress.dir/build: compress
.PHONY : CMakeFiles/compress.dir/build

CMakeFiles/compress.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/compress.dir/cmake_clean.cmake
.PHONY : CMakeFiles/compress.dir/clean

CMakeFiles/compress.dir/depend:
	cd /home/gleb/compression/src/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gleb/compression/src /home/gleb/compression/src /home/gleb/compression/src/cmake-build-debug /home/gleb/compression/src/cmake-build-debug /home/gleb/compression/src/cmake-build-debug/CMakeFiles/compress.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/compress.dir/depend

