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

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/projects/CoroutineDFS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/projects/CoroutineDFS/build

# Include any dependencies generated for this target.
include CMakeFiles/mainDFS.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/mainDFS.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/mainDFS.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mainDFS.dir/flags.make

CMakeFiles/mainDFS.dir/sylar/main.cc.o: CMakeFiles/mainDFS.dir/flags.make
CMakeFiles/mainDFS.dir/sylar/main.cc.o: ../sylar/main.cc
CMakeFiles/mainDFS.dir/sylar/main.cc.o: CMakeFiles/mainDFS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/projects/CoroutineDFS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mainDFS.dir/sylar/main.cc.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/mainDFS.dir/sylar/main.cc.o -MF CMakeFiles/mainDFS.dir/sylar/main.cc.o.d -o CMakeFiles/mainDFS.dir/sylar/main.cc.o -c /root/projects/CoroutineDFS/sylar/main.cc

CMakeFiles/mainDFS.dir/sylar/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mainDFS.dir/sylar/main.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/projects/CoroutineDFS/sylar/main.cc > CMakeFiles/mainDFS.dir/sylar/main.cc.i

CMakeFiles/mainDFS.dir/sylar/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mainDFS.dir/sylar/main.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/projects/CoroutineDFS/sylar/main.cc -o CMakeFiles/mainDFS.dir/sylar/main.cc.s

# Object files for target mainDFS
mainDFS_OBJECTS = \
"CMakeFiles/mainDFS.dir/sylar/main.cc.o"

# External object files for target mainDFS
mainDFS_EXTERNAL_OBJECTS =

../bin/mainDFS: CMakeFiles/mainDFS.dir/sylar/main.cc.o
../bin/mainDFS: CMakeFiles/mainDFS.dir/build.make
../bin/mainDFS: ../lib/libsylar.so
../bin/mainDFS: /usr/lib64/libpthread.so
../bin/mainDFS: /usr/local/lib64/libyaml-cpp.so
../bin/mainDFS: CMakeFiles/mainDFS.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/projects/CoroutineDFS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/mainDFS"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mainDFS.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mainDFS.dir/build: ../bin/mainDFS
.PHONY : CMakeFiles/mainDFS.dir/build

CMakeFiles/mainDFS.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mainDFS.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mainDFS.dir/clean

CMakeFiles/mainDFS.dir/depend:
	cd /root/projects/CoroutineDFS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/projects/CoroutineDFS /root/projects/CoroutineDFS /root/projects/CoroutineDFS/build /root/projects/CoroutineDFS/build /root/projects/CoroutineDFS/build/CMakeFiles/mainDFS.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mainDFS.dir/depend

