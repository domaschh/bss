# ----------------------------------------------------------------------------
# Makefile for Vigenere Cipher
#
# Author: Thomas Mages
# Email: 212124528@student.tuwien.ac.at
# Date: 18.10.2023
# Description: This Makefile compiles and builds the Vigenere Cipher project.
# ----------------------------------------------------------------------------

CC     := gcc
CFLAGS := -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
          -D_POSIX_C_SOURCE=200809L -g
LFLAGS :=

SRC := vigenere.c
OBJ := vigenere.o
BIN := vigenere

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LFLAGS) $^ -o $@

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
