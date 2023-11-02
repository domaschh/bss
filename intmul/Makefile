# ----------------------------------------------------------------------------
# Makefile for Intmul
#
# Author: Thomas Mages
# Email: 212124528@student.tuwien.ac.at
# Date: 27.10.2023
# Description: This Makefile compiles and builds the intmul project
# ----------------------------------------------------------------------------


# Variables
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
          -D_POSIX_C_SOURCE=200809L -g
          OBJS = intmul.o
#
#          # Default rule
all: intmul

# Linking rule
intmul: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
#
#     # Compilation rules
intmul.o: intmul.c intmul.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule
clean:
	$ rm -f intmul intmul.o
#
.PHONY: all clean
#
