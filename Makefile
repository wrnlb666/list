CC = gcc
CFLAG = -Wall -Wextra -Wpedantic -std=c2x -g
DIR = src
OBJ = slice.o
LIB = 
POST_FIX = 
ELF_FILES = 

ifeq ($(OS),Windows_NT)
	POST_FIX = dll
else
	UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAG += -Wno-unused-result
		POST_FIX = so
		ELF_FILES := $(shell find . -type f -executable -exec sh -c 'file -b {} | grep -q ELF' \; -print)
    endif
endif

all: slice test
.PHONY: slice test

slice: $(DIR)/slice.c $(DIR)/slice.h
	$(CC) $(CFLAG) $< -c

test: test.c
	$(CC) $(CFLAG) $(OBJ) $< -o $@

clean:
	rm *.dll *.exe *.o $(ELF_FILES)