CC = gcc
CFLAG = -Wall -Wextra -Wpedantic -std=c2x -g
DIR = src
OBJ = list.o
LIB = 
POST_FIX = 
ELF_FILES = 

ifeq ($(OS),Windows_NT)
	POST_FIX = dll
	LIB = -L. -llist
else
	UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAG += -Wno-unused-result
		POST_FIX = so
		LIB = -L. -llist
		ELF_FILES := $(shell find . -type f -executable -exec sh -c 'file -b {} | grep -q ELF' \; -print)
    endif
endif

all: list
.PHONY: list

list: $(DIR)/list.c $(DIR)/list.h
	$(CC) $(CFLAG) -fPIC -shared $< -o lib$@.$(POST_FIX)

test%: test%.c
	$(CC) $(CFLAG) -Wl,-rpath=./ $< -o test $(LIB)

clean:
	rm *.dll *.exe *.o $(ELF_FILES)
