# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /home/lingjie/cmpt433/work/CMPT433_ALLIN

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lingjie/cmpt433/work/CMPT433_ALLIN/build

# Include any dependencies generated for this target.
include app/CMakeFiles/allin.dir/depend.make

# Include the progress variables for this target.
include app/CMakeFiles/allin.dir/progress.make

# Include the compile flags for this target's objects.
include app/CMakeFiles/allin.dir/flags.make

app/CMakeFiles/allin.dir/src/main.c.o: app/CMakeFiles/allin.dir/flags.make
app/CMakeFiles/allin.dir/src/main.c.o: ../app/src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lingjie/cmpt433/work/CMPT433_ALLIN/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object app/CMakeFiles/allin.dir/src/main.c.o"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && /usr/bin/arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/allin.dir/src/main.c.o -c /home/lingjie/cmpt433/work/CMPT433_ALLIN/app/src/main.c

app/CMakeFiles/allin.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/allin.dir/src/main.c.i"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && /usr/bin/arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lingjie/cmpt433/work/CMPT433_ALLIN/app/src/main.c > CMakeFiles/allin.dir/src/main.c.i

app/CMakeFiles/allin.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/allin.dir/src/main.c.s"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && /usr/bin/arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lingjie/cmpt433/work/CMPT433_ALLIN/app/src/main.c -o CMakeFiles/allin.dir/src/main.c.s

app/CMakeFiles/allin.dir/src/threadController.c.o: app/CMakeFiles/allin.dir/flags.make
app/CMakeFiles/allin.dir/src/threadController.c.o: ../app/src/threadController.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lingjie/cmpt433/work/CMPT433_ALLIN/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object app/CMakeFiles/allin.dir/src/threadController.c.o"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && /usr/bin/arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/allin.dir/src/threadController.c.o -c /home/lingjie/cmpt433/work/CMPT433_ALLIN/app/src/threadController.c

app/CMakeFiles/allin.dir/src/threadController.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/allin.dir/src/threadController.c.i"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && /usr/bin/arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lingjie/cmpt433/work/CMPT433_ALLIN/app/src/threadController.c > CMakeFiles/allin.dir/src/threadController.c.i

app/CMakeFiles/allin.dir/src/threadController.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/allin.dir/src/threadController.c.s"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && /usr/bin/arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lingjie/cmpt433/work/CMPT433_ALLIN/app/src/threadController.c -o CMakeFiles/allin.dir/src/threadController.c.s

# Object files for target allin
allin_OBJECTS = \
"CMakeFiles/allin.dir/src/main.c.o" \
"CMakeFiles/allin.dir/src/threadController.c.o"

# External object files for target allin
allin_EXTERNAL_OBJECTS =

app/allin: app/CMakeFiles/allin.dir/src/main.c.o
app/allin: app/CMakeFiles/allin.dir/src/threadController.c.o
app/allin: app/CMakeFiles/allin.dir/build.make
app/allin: hal/libhal.a
app/allin: app/CMakeFiles/allin.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lingjie/cmpt433/work/CMPT433_ALLIN/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable allin"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/allin.dir/link.txt --verbose=$(VERBOSE)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Copying ARM executable to public NFS directory"
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && /usr/bin/cmake -E copy /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app/allin ~/cmpt433/public/myApps/allin

# Rule to build all files generated by this target.
app/CMakeFiles/allin.dir/build: app/allin

.PHONY : app/CMakeFiles/allin.dir/build

app/CMakeFiles/allin.dir/clean:
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app && $(CMAKE_COMMAND) -P CMakeFiles/allin.dir/cmake_clean.cmake
.PHONY : app/CMakeFiles/allin.dir/clean

app/CMakeFiles/allin.dir/depend:
	cd /home/lingjie/cmpt433/work/CMPT433_ALLIN/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lingjie/cmpt433/work/CMPT433_ALLIN /home/lingjie/cmpt433/work/CMPT433_ALLIN/app /home/lingjie/cmpt433/work/CMPT433_ALLIN/build /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app /home/lingjie/cmpt433/work/CMPT433_ALLIN/build/app/CMakeFiles/allin.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : app/CMakeFiles/allin.dir/depend

