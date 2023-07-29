# Makefile based on https://github.com/CrusaderSVK287/Makefiles/blob/main/C_projects/Makefile

#################################### USAGE #####################################

# Do not remove any variables from this section,
# If you dont want to use them, just leave them
# empty like the LDLIBS variable

# CC - used C compiler
# LDLIBS - libraries to be linked, for example -lm
# CFLAGS - flags used to compile program like -Wall

# Repository structure for this makefile to work correctly
# 1. All source files must be under SRCDIR directory
# 2. Files with source code for testing must be in TESTDIR directory

# You can visit this makefile repository and example project here
# https://github.com/CrusaderSVK287/Makefiles/tree/main/C_projects

################################### TARGETS ####################################

# all: compiles source code into object files and result binary, call just make
# test: compiles if necesarry, executes tests
# clean: cleans repository of temporary files, object files and binaries
# deps: lists dependencies needed for full makefile functionality

################################ CONFIGURATION #################################
# Compiler flags
CC = gcc
LDLIBS =
CFLAGS = -Wall -Werror

# Source and build directories ending with /
# DO NOT ADD SPACE AFTER /, COULD RESULT IN UNWANTED DELETION OF FILES
SRCDIR  = src/
OBJDIR  = obj/
BINDIR  = bin/
TESTDIR = test/

# Result binary name
BINNAME = libcclog.so

################################################################################
#                                                                              #
#                  DO NOT CHANGE ANYTHING BELOW THIS LINE                      #
#                                                                              #
################################################################################

### COMPILATION ###

SRCS  = $(shell find $(SRCDIR) -name '*.c')
OBJS  = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRCS))
TSRCS = $(shell find $(TESTDIR) -name '*.c')
TOBJS = $(patsubst $(TESTDIR)%.c, $(OBJDIR)$(TESTDIR)%.o, $(TSRCS)) \
	$(subst $(OBJDIR)main.o,,$(OBJS))

BIN = $(BINDIR)$(BINNAME)

all: -setup $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDLIBS) $(OBJS) $(TESTFLAGS) -o $@
	@echo -e "\n\nBuild successful, result generated in $(PWD)/$(BINDIR)\n\n\n"

$(OBJDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $(CFLAGS) $(TESTFLAGS) $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

### TESTS ###
$(OBJDIR)$(TESTDIR)%.o: $(TESTDIR)%.c
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS)  -I $(SRCDIR) -c $< -o $@

test: -setup $(OBJS) $(TOBJS)
	$(CC) $(LDLIBS) -I $(SRCDIR) $(TOBJS) -o $(BINDIR)$@
	@echo "----- RUNNING TESTS -----"
	$(BINDIR)$@

### SET UP ###
-setup:
	mkdir -p $(SRCDIR) $(OBJDIR) $(BINDIR) $(TESTDIR)

### DEPENDENCIES ###
deps:
	@echo "Dependencies: make, $(CC)"
	@echo "Libraries needed: $(LDLIBS)"

