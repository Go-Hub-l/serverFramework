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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/CPP/sylar/workspace/serverFramework

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/CPP/sylar/workspace/serverFramework/build

# Include any dependencies generated for this target.
include CMakeFiles/main.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/main.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.dir/flags.make

CMakeFiles/main.dir/sylar/main.cc.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/sylar/main.cc.o: ../sylar/main.cc
CMakeFiles/main.dir/sylar/main.cc.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/CPP/sylar/workspace/serverFramework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main.dir/sylar/main.cc.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/sylar/main.cc.o -MF CMakeFiles/main.dir/sylar/main.cc.o.d -o CMakeFiles/main.dir/sylar/main.cc.o -c /root/CPP/sylar/workspace/serverFramework/sylar/main.cc

CMakeFiles/main.dir/sylar/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/sylar/main.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/CPP/sylar/workspace/serverFramework/sylar/main.cc > CMakeFiles/main.dir/sylar/main.cc.i

CMakeFiles/main.dir/sylar/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/sylar/main.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/CPP/sylar/workspace/serverFramework/sylar/main.cc -o CMakeFiles/main.dir/sylar/main.cc.s

# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/sylar/main.cc.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

../bin/main: CMakeFiles/main.dir/sylar/main.cc.o
../bin/main: CMakeFiles/main.dir/build.make
../bin/main: ../lib/libsylar.so
../bin/main: /usr/lib64/libpthread.so
../bin/main: /usr/local/lib64/libyaml-cpp.so
../bin/main: CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/CPP/sylar/workspace/serverFramework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.dir/build: ../bin/main
.PHONY : CMakeFiles/main.dir/build

CMakeFiles/main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.dir/clean

CMakeFiles/main.dir/depend:
	cd /root/CPP/sylar/workspace/serverFramework/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/CPP/sylar/workspace/serverFramework /root/CPP/sylar/workspace/serverFramework /root/CPP/sylar/workspace/serverFramework/build /root/CPP/sylar/workspace/serverFramework/build /root/CPP/sylar/workspace/serverFramework/build/CMakeFiles/main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main.dir/depend
