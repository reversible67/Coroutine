# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = /usr/local/cmake/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wang/Coroutine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wang/Coroutine/build

# Include any dependencies generated for this target.
include CMakeFiles/duan.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/duan.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/duan.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/duan.dir/flags.make

CMakeFiles/duan.dir/duan_src/log.cc.o: CMakeFiles/duan.dir/flags.make
CMakeFiles/duan.dir/duan_src/log.cc.o: /home/wang/Coroutine/duan_src/log.cc
CMakeFiles/duan.dir/duan_src/log.cc.o: CMakeFiles/duan.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wang/Coroutine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/duan.dir/duan_src/log.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/duan.dir/duan_src/log.cc.o -MF CMakeFiles/duan.dir/duan_src/log.cc.o.d -o CMakeFiles/duan.dir/duan_src/log.cc.o -c /home/wang/Coroutine/duan_src/log.cc

CMakeFiles/duan.dir/duan_src/log.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/duan.dir/duan_src/log.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wang/Coroutine/duan_src/log.cc > CMakeFiles/duan.dir/duan_src/log.cc.i

CMakeFiles/duan.dir/duan_src/log.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/duan.dir/duan_src/log.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wang/Coroutine/duan_src/log.cc -o CMakeFiles/duan.dir/duan_src/log.cc.s

CMakeFiles/duan.dir/duan_src/util.cc.o: CMakeFiles/duan.dir/flags.make
CMakeFiles/duan.dir/duan_src/util.cc.o: /home/wang/Coroutine/duan_src/util.cc
CMakeFiles/duan.dir/duan_src/util.cc.o: CMakeFiles/duan.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wang/Coroutine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/duan.dir/duan_src/util.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/duan.dir/duan_src/util.cc.o -MF CMakeFiles/duan.dir/duan_src/util.cc.o.d -o CMakeFiles/duan.dir/duan_src/util.cc.o -c /home/wang/Coroutine/duan_src/util.cc

CMakeFiles/duan.dir/duan_src/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/duan.dir/duan_src/util.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wang/Coroutine/duan_src/util.cc > CMakeFiles/duan.dir/duan_src/util.cc.i

CMakeFiles/duan.dir/duan_src/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/duan.dir/duan_src/util.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wang/Coroutine/duan_src/util.cc -o CMakeFiles/duan.dir/duan_src/util.cc.s

CMakeFiles/duan.dir/duan_src/config.cc.o: CMakeFiles/duan.dir/flags.make
CMakeFiles/duan.dir/duan_src/config.cc.o: /home/wang/Coroutine/duan_src/config.cc
CMakeFiles/duan.dir/duan_src/config.cc.o: CMakeFiles/duan.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wang/Coroutine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/duan.dir/duan_src/config.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/duan.dir/duan_src/config.cc.o -MF CMakeFiles/duan.dir/duan_src/config.cc.o.d -o CMakeFiles/duan.dir/duan_src/config.cc.o -c /home/wang/Coroutine/duan_src/config.cc

CMakeFiles/duan.dir/duan_src/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/duan.dir/duan_src/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wang/Coroutine/duan_src/config.cc > CMakeFiles/duan.dir/duan_src/config.cc.i

CMakeFiles/duan.dir/duan_src/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/duan.dir/duan_src/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wang/Coroutine/duan_src/config.cc -o CMakeFiles/duan.dir/duan_src/config.cc.s

# Object files for target duan
duan_OBJECTS = \
"CMakeFiles/duan.dir/duan_src/log.cc.o" \
"CMakeFiles/duan.dir/duan_src/util.cc.o" \
"CMakeFiles/duan.dir/duan_src/config.cc.o"

# External object files for target duan
duan_EXTERNAL_OBJECTS =

/home/wang/Coroutine/lib/libduan.so: CMakeFiles/duan.dir/duan_src/log.cc.o
/home/wang/Coroutine/lib/libduan.so: CMakeFiles/duan.dir/duan_src/util.cc.o
/home/wang/Coroutine/lib/libduan.so: CMakeFiles/duan.dir/duan_src/config.cc.o
/home/wang/Coroutine/lib/libduan.so: CMakeFiles/duan.dir/build.make
/home/wang/Coroutine/lib/libduan.so: CMakeFiles/duan.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/wang/Coroutine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX shared library /home/wang/Coroutine/lib/libduan.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/duan.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/duan.dir/build: /home/wang/Coroutine/lib/libduan.so
.PHONY : CMakeFiles/duan.dir/build

CMakeFiles/duan.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/duan.dir/cmake_clean.cmake
.PHONY : CMakeFiles/duan.dir/clean

CMakeFiles/duan.dir/depend:
	cd /home/wang/Coroutine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wang/Coroutine /home/wang/Coroutine /home/wang/Coroutine/build /home/wang/Coroutine/build /home/wang/Coroutine/build/CMakeFiles/duan.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/duan.dir/depend

