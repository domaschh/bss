# ----------------------------------------------------------------------------
# Makefile for Intmul
#
# Author: Thomas Mages
# Email: e12124528@student.tuwien.ac.at
# Date: 4.11.2023
# Description: This Makefile compiles and builds the intmul project
# ----------------------------------------------------------------------------


# Variables
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
          -D_POSIX_C_SOURCE=200809L -g
          OBJS = intmul.o

all: intmul

intmul: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

intmul.o: intmul.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$ rm -f intmul intmul.o

.PHONY: all clean
