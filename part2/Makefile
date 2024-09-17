# Source: Professor Jae Woo Lee's Makefile template we were given in Advanced Programming.
# Source link: https://cs3157.github.io/www/2024-1/lecture-notes/02-compile.pdf
CC = gcc

# Compilation options
CFLAGS = -g -Wall
# Linking options:
LDFLAGS =
# Library options:
LDLIBS =

w4118_sh2: shell2.o 
	$(CC) $(LDFLAGS) $(LDLIBS) -o w4118_sh2 shell2.o

# Make uses the following implicit rule to compile a .c file into a .o file:
#
# $(CC) -c $(CFLAGS) -o <target-.o-file> <the-.c-file>

shell2.o: shell2.h


# Always provide the "clean" target that removes build artifacts (e.g., the
# target executable and .o files) and other garbage that may be created during
# the development process.

# The "clean" target does not correspond to a filename, so we tell Make that
# it’s a "phony" target, meaning it does not need to check the timestamp of
# a file called "clean".
.PHONY: clean
clean:
	rm -f *.o a.out core w4118_sh2
# The "all" target can be useful if your Makefile builds multiple programs.
# Here we’ll have it first do "clean", and rebuild the myprogram target.
#
# Like "clean", we declare it as a phony target because it doesn’t actually
# build a file named "all".
#
# Dependencies are built in the order they are declared, left to right.
.PHONY: all
	all: clean w4118_sh2
