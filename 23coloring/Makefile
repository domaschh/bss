# ----------------------------------------------------------------------------
# Makefile for 33coloring
#
# Author: Thomas Mages
# Email: e12124528@student.tuwien.ac.at
# Date: 10.11.2023
# Description: This Makefile compiles and builds the 3coloring project
# ----------------------------------------------------------------------------

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
                 -D_POSIX_C_SOURCE=200809L -g
                 OBJS = intmul.o


# All targets
all: supervisor generator

# Rule for building supervisor
supervisor: supervisor.c
	$(CC) $(CFLAGS) -o supervisor supervisor.c

# Rule for building generator
generator: generator.c
	$(CC) $(CFLAGS) -o generator generator.c

# Clean rule
clean:
	rm -f supervisor generator