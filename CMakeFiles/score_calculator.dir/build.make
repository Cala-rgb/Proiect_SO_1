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
CMAKE_SOURCE_DIR = /home/cala

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cala

# Include any dependencies generated for this target.
include CMakeFiles/score_calculator.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/score_calculator.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/score_calculator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/score_calculator.dir/flags.make

CMakeFiles/score_calculator.dir/score_calculator.c.o: CMakeFiles/score_calculator.dir/flags.make
CMakeFiles/score_calculator.dir/score_calculator.c.o: score_calculator.c
CMakeFiles/score_calculator.dir/score_calculator.c.o: CMakeFiles/score_calculator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/cala/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/score_calculator.dir/score_calculator.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/score_calculator.dir/score_calculator.c.o -MF CMakeFiles/score_calculator.dir/score_calculator.c.o.d -o CMakeFiles/score_calculator.dir/score_calculator.c.o -c /home/cala/score_calculator.c

CMakeFiles/score_calculator.dir/score_calculator.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/score_calculator.dir/score_calculator.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/cala/score_calculator.c > CMakeFiles/score_calculator.dir/score_calculator.c.i

CMakeFiles/score_calculator.dir/score_calculator.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/score_calculator.dir/score_calculator.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/cala/score_calculator.c -o CMakeFiles/score_calculator.dir/score_calculator.c.s

# Object files for target score_calculator
score_calculator_OBJECTS = \
"CMakeFiles/score_calculator.dir/score_calculator.c.o"

# External object files for target score_calculator
score_calculator_EXTERNAL_OBJECTS =

score_calculator: CMakeFiles/score_calculator.dir/score_calculator.c.o
score_calculator: CMakeFiles/score_calculator.dir/build.make
score_calculator: CMakeFiles/score_calculator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/cala/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable score_calculator"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/score_calculator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/score_calculator.dir/build: score_calculator
.PHONY : CMakeFiles/score_calculator.dir/build

CMakeFiles/score_calculator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/score_calculator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/score_calculator.dir/clean

CMakeFiles/score_calculator.dir/depend:
	cd /home/cala && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cala /home/cala /home/cala /home/cala /home/cala/CMakeFiles/score_calculator.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/score_calculator.dir/depend

