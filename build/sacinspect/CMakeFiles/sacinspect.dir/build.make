# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home_ad/israel/sactools

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home_ad/israel/sactools/build

# Include any dependencies generated for this target.
include sacinspect/CMakeFiles/sacinspect.dir/depend.make

# Include the progress variables for this target.
include sacinspect/CMakeFiles/sacinspect.dir/progress.make

# Include the compile flags for this target's objects.
include sacinspect/CMakeFiles/sacinspect.dir/flags.make

sacinspect/CMakeFiles/sacinspect.dir/EX.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/EX.c.o: ../sacinspect/EX.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/EX.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/EX.c.o   -c /home_ad/israel/sactools/sacinspect/EX.c

sacinspect/CMakeFiles/sacinspect.dir/EX.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/EX.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/EX.c > CMakeFiles/sacinspect.dir/EX.c.i

sacinspect/CMakeFiles/sacinspect.dir/EX.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/EX.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/EX.c -o CMakeFiles/sacinspect.dir/EX.c.s

sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/EX.c.o

sacinspect/CMakeFiles/sacinspect.dir/PK.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/PK.c.o: ../sacinspect/PK.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/PK.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/PK.c.o   -c /home_ad/israel/sactools/sacinspect/PK.c

sacinspect/CMakeFiles/sacinspect.dir/PK.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/PK.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/PK.c > CMakeFiles/sacinspect.dir/PK.c.i

sacinspect/CMakeFiles/sacinspect.dir/PK.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/PK.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/PK.c -o CMakeFiles/sacinspect.dir/PK.c.s

sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/PK.c.o

sacinspect/CMakeFiles/sacinspect.dir/aux.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/aux.c.o: ../sacinspect/aux.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/aux.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/aux.c.o   -c /home_ad/israel/sactools/sacinspect/aux.c

sacinspect/CMakeFiles/sacinspect.dir/aux.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/aux.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/aux.c > CMakeFiles/sacinspect.dir/aux.c.i

sacinspect/CMakeFiles/sacinspect.dir/aux.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/aux.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/aux.c -o CMakeFiles/sacinspect.dir/aux.c.s

sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/aux.c.o

sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o: ../sacinspect/edit_tf.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/edit_tf.c.o   -c /home_ad/israel/sactools/sacinspect/edit_tf.c

sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/edit_tf.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/edit_tf.c > CMakeFiles/sacinspect.dir/edit_tf.c.i

sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/edit_tf.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/edit_tf.c -o CMakeFiles/sacinspect.dir/edit_tf.c.s

sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o

sacinspect/CMakeFiles/sacinspect.dir/io.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/io.c.o: ../sacinspect/io.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/io.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/io.c.o   -c /home_ad/israel/sactools/sacinspect/io.c

sacinspect/CMakeFiles/sacinspect.dir/io.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/io.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/io.c > CMakeFiles/sacinspect.dir/io.c.i

sacinspect/CMakeFiles/sacinspect.dir/io.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/io.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/io.c -o CMakeFiles/sacinspect.dir/io.c.s

sacinspect/CMakeFiles/sacinspect.dir/io.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/io.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/io.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/io.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/io.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/io.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/io.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/io.c.o

sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o: ../sacinspect/sacinspect.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/sacinspect.c.o   -c /home_ad/israel/sactools/sacinspect/sacinspect.c

sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/sacinspect.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/sacinspect.c > CMakeFiles/sacinspect.dir/sacinspect.c.i

sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/sacinspect.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/sacinspect.c -o CMakeFiles/sacinspect.dir/sacinspect.c.s

sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o

sacinspect/CMakeFiles/sacinspect.dir/collector.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/collector.c.o: ../sacinspect/collector.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/collector.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/collector.c.o   -c /home_ad/israel/sactools/sacinspect/collector.c

sacinspect/CMakeFiles/sacinspect.dir/collector.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/collector.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/collector.c > CMakeFiles/sacinspect.dir/collector.c.i

sacinspect/CMakeFiles/sacinspect.dir/collector.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/collector.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/collector.c -o CMakeFiles/sacinspect.dir/collector.c.s

sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/collector.c.o

sacinspect/CMakeFiles/sacinspect.dir/globers.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/globers.c.o: ../sacinspect/globers.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/globers.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/globers.c.o   -c /home_ad/israel/sactools/sacinspect/globers.c

sacinspect/CMakeFiles/sacinspect.dir/globers.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/globers.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/globers.c > CMakeFiles/sacinspect.dir/globers.c.i

sacinspect/CMakeFiles/sacinspect.dir/globers.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/globers.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/globers.c -o CMakeFiles/sacinspect.dir/globers.c.s

sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/globers.c.o

sacinspect/CMakeFiles/sacinspect.dir/HS.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/HS.c.o: ../sacinspect/HS.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/HS.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/HS.c.o   -c /home_ad/israel/sactools/sacinspect/HS.c

sacinspect/CMakeFiles/sacinspect.dir/HS.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/HS.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/HS.c > CMakeFiles/sacinspect.dir/HS.c.i

sacinspect/CMakeFiles/sacinspect.dir/HS.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/HS.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/HS.c -o CMakeFiles/sacinspect.dir/HS.c.s

sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/HS.c.o

sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o: ../sacinspect/MAP.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_10)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/MAP.c.o   -c /home_ad/israel/sactools/sacinspect/MAP.c

sacinspect/CMakeFiles/sacinspect.dir/MAP.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/MAP.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/MAP.c > CMakeFiles/sacinspect.dir/MAP.c.i

sacinspect/CMakeFiles/sacinspect.dir/MAP.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/MAP.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/MAP.c -o CMakeFiles/sacinspect.dir/MAP.c.s

sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o

sacinspect/CMakeFiles/sacinspect.dir/correl.c.o: sacinspect/CMakeFiles/sacinspect.dir/flags.make
sacinspect/CMakeFiles/sacinspect.dir/correl.c.o: ../sacinspect/correl.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home_ad/israel/sactools/build/CMakeFiles $(CMAKE_PROGRESS_11)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object sacinspect/CMakeFiles/sacinspect.dir/correl.c.o"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/sacinspect.dir/correl.c.o   -c /home_ad/israel/sactools/sacinspect/correl.c

sacinspect/CMakeFiles/sacinspect.dir/correl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sacinspect.dir/correl.c.i"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home_ad/israel/sactools/sacinspect/correl.c > CMakeFiles/sacinspect.dir/correl.c.i

sacinspect/CMakeFiles/sacinspect.dir/correl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sacinspect.dir/correl.c.s"
	cd /home_ad/israel/sactools/build/sacinspect && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home_ad/israel/sactools/sacinspect/correl.c -o CMakeFiles/sacinspect.dir/correl.c.s

sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.requires:
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.requires

sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.provides: sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.requires
	$(MAKE) -f sacinspect/CMakeFiles/sacinspect.dir/build.make sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.provides.build
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.provides

sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.provides.build: sacinspect/CMakeFiles/sacinspect.dir/correl.c.o

# Object files for target sacinspect
sacinspect_OBJECTS = \
"CMakeFiles/sacinspect.dir/EX.c.o" \
"CMakeFiles/sacinspect.dir/PK.c.o" \
"CMakeFiles/sacinspect.dir/aux.c.o" \
"CMakeFiles/sacinspect.dir/edit_tf.c.o" \
"CMakeFiles/sacinspect.dir/io.c.o" \
"CMakeFiles/sacinspect.dir/sacinspect.c.o" \
"CMakeFiles/sacinspect.dir/collector.c.o" \
"CMakeFiles/sacinspect.dir/globers.c.o" \
"CMakeFiles/sacinspect.dir/HS.c.o" \
"CMakeFiles/sacinspect.dir/MAP.c.o" \
"CMakeFiles/sacinspect.dir/correl.c.o"

# External object files for target sacinspect
sacinspect_EXTERNAL_OBJECTS =

sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/EX.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/PK.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/aux.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/io.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/collector.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/globers.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/HS.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/correl.c.o
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/build.make
sacinspect/sacinspect: mblibsac/libmblibsac.a
sacinspect/sacinspect: mblibgr/libmblibgr.a
sacinspect/sacinspect: /usr/lib/x86_64-linux-gnu/libSM.so
sacinspect/sacinspect: /usr/lib/x86_64-linux-gnu/libICE.so
sacinspect/sacinspect: /usr/lib/x86_64-linux-gnu/libX11.so
sacinspect/sacinspect: /usr/lib/x86_64-linux-gnu/libXext.so
sacinspect/sacinspect: sacinspect/CMakeFiles/sacinspect.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable sacinspect"
	cd /home_ad/israel/sactools/build/sacinspect && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sacinspect.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
sacinspect/CMakeFiles/sacinspect.dir/build: sacinspect/sacinspect
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/build

sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/EX.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/PK.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/aux.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/edit_tf.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/io.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/sacinspect.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/collector.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/globers.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/HS.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/MAP.c.o.requires
sacinspect/CMakeFiles/sacinspect.dir/requires: sacinspect/CMakeFiles/sacinspect.dir/correl.c.o.requires
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/requires

sacinspect/CMakeFiles/sacinspect.dir/clean:
	cd /home_ad/israel/sactools/build/sacinspect && $(CMAKE_COMMAND) -P CMakeFiles/sacinspect.dir/cmake_clean.cmake
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/clean

sacinspect/CMakeFiles/sacinspect.dir/depend:
	cd /home_ad/israel/sactools/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home_ad/israel/sactools /home_ad/israel/sactools/sacinspect /home_ad/israel/sactools/build /home_ad/israel/sactools/build/sacinspect /home_ad/israel/sactools/build/sacinspect/CMakeFiles/sacinspect.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : sacinspect/CMakeFiles/sacinspect.dir/depend
