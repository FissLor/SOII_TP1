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
CMAKE_COMMAND = /snap/clion/111/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/111/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lfiss/CLionProjects/TP1/Server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lfiss/CLionProjects/TP1/Server

# Include any dependencies generated for this target.
include Common/Util/CMakeFiles/Util.dir/depend.make

# Include the progress variables for this target.
include Common/Util/CMakeFiles/Util.dir/progress.make

# Include the compile flags for this target's objects.
include Common/Util/CMakeFiles/Util.dir/flags.make

Common/Util/CMakeFiles/Util.dir/util.c.o: Common/Util/CMakeFiles/Util.dir/flags.make
Common/Util/CMakeFiles/Util.dir/util.c.o: Common/Util/util.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lfiss/CLionProjects/TP1/Server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object Common/Util/CMakeFiles/Util.dir/util.c.o"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Util && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Util.dir/util.c.o   -c /home/lfiss/CLionProjects/TP1/Server/Common/Util/util.c

Common/Util/CMakeFiles/Util.dir/util.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Util.dir/util.c.i"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Util && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lfiss/CLionProjects/TP1/Server/Common/Util/util.c > CMakeFiles/Util.dir/util.c.i

Common/Util/CMakeFiles/Util.dir/util.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Util.dir/util.c.s"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Util && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lfiss/CLionProjects/TP1/Server/Common/Util/util.c -o CMakeFiles/Util.dir/util.c.s

# Object files for target Util
Util_OBJECTS = \
"CMakeFiles/Util.dir/util.c.o"

# External object files for target Util
Util_EXTERNAL_OBJECTS =

Common/Util/libUtil.a: Common/Util/CMakeFiles/Util.dir/util.c.o
Common/Util/libUtil.a: Common/Util/CMakeFiles/Util.dir/build.make
Common/Util/libUtil.a: Common/Util/CMakeFiles/Util.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lfiss/CLionProjects/TP1/Server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libUtil.a"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Util && $(CMAKE_COMMAND) -P CMakeFiles/Util.dir/cmake_clean_target.cmake
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Util && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Util.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Common/Util/CMakeFiles/Util.dir/build: Common/Util/libUtil.a

.PHONY : Common/Util/CMakeFiles/Util.dir/build

Common/Util/CMakeFiles/Util.dir/clean:
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Util && $(CMAKE_COMMAND) -P CMakeFiles/Util.dir/cmake_clean.cmake
.PHONY : Common/Util/CMakeFiles/Util.dir/clean

Common/Util/CMakeFiles/Util.dir/depend:
	cd /home/lfiss/CLionProjects/TP1/Server && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lfiss/CLionProjects/TP1/Server /home/lfiss/CLionProjects/TP1/Server/Common/Util /home/lfiss/CLionProjects/TP1/Server /home/lfiss/CLionProjects/TP1/Server/Common/Util /home/lfiss/CLionProjects/TP1/Server/Common/Util/CMakeFiles/Util.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Common/Util/CMakeFiles/Util.dir/depend

