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
include Common/Hash/CMakeFiles/Hash.dir/depend.make

# Include the progress variables for this target.
include Common/Hash/CMakeFiles/Hash.dir/progress.make

# Include the compile flags for this target's objects.
include Common/Hash/CMakeFiles/Hash.dir/flags.make

Common/Hash/CMakeFiles/Hash.dir/hash.c.o: Common/Hash/CMakeFiles/Hash.dir/flags.make
Common/Hash/CMakeFiles/Hash.dir/hash.c.o: Common/Hash/hash.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lfiss/CLionProjects/TP1/Server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object Common/Hash/CMakeFiles/Hash.dir/hash.c.o"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Hash && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Hash.dir/hash.c.o   -c /home/lfiss/CLionProjects/TP1/Server/Common/Hash/hash.c

Common/Hash/CMakeFiles/Hash.dir/hash.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Hash.dir/hash.c.i"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Hash && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lfiss/CLionProjects/TP1/Server/Common/Hash/hash.c > CMakeFiles/Hash.dir/hash.c.i

Common/Hash/CMakeFiles/Hash.dir/hash.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Hash.dir/hash.c.s"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Hash && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lfiss/CLionProjects/TP1/Server/Common/Hash/hash.c -o CMakeFiles/Hash.dir/hash.c.s

# Object files for target Hash
Hash_OBJECTS = \
"CMakeFiles/Hash.dir/hash.c.o"

# External object files for target Hash
Hash_EXTERNAL_OBJECTS =

Common/Hash/libHash.a: Common/Hash/CMakeFiles/Hash.dir/hash.c.o
Common/Hash/libHash.a: Common/Hash/CMakeFiles/Hash.dir/build.make
Common/Hash/libHash.a: Common/Hash/CMakeFiles/Hash.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lfiss/CLionProjects/TP1/Server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libHash.a"
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Hash && $(CMAKE_COMMAND) -P CMakeFiles/Hash.dir/cmake_clean_target.cmake
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Hash && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Hash.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Common/Hash/CMakeFiles/Hash.dir/build: Common/Hash/libHash.a

.PHONY : Common/Hash/CMakeFiles/Hash.dir/build

Common/Hash/CMakeFiles/Hash.dir/clean:
	cd /home/lfiss/CLionProjects/TP1/Server/Common/Hash && $(CMAKE_COMMAND) -P CMakeFiles/Hash.dir/cmake_clean.cmake
.PHONY : Common/Hash/CMakeFiles/Hash.dir/clean

Common/Hash/CMakeFiles/Hash.dir/depend:
	cd /home/lfiss/CLionProjects/TP1/Server && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lfiss/CLionProjects/TP1/Server /home/lfiss/CLionProjects/TP1/Server/Common/Hash /home/lfiss/CLionProjects/TP1/Server /home/lfiss/CLionProjects/TP1/Server/Common/Hash /home/lfiss/CLionProjects/TP1/Server/Common/Hash/CMakeFiles/Hash.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Common/Hash/CMakeFiles/Hash.dir/depend
