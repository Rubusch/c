# Makefile
SOURCE = lib_sock__test.c lib_sock.c lib_streams__test.c lib_streams.c lib_unix__test.c lib_unix.c lib_error.c
OBJECTS = $(SOURCE:.c=.o)


.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall #-Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

all: lib_sock__test.exe lib_streams__test.exe lib_unix__test.exe


# TODO further lib_ targets

lib_sock__test.exe: lib_sock__test.o lib_sock.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_streams__test.exe: lib_streams__test.o lib_streams.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_unix__test.exe: lib_unix__test.o lib_unix.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^



clean:
	$(RM) $(OBJECTS) *.exe *~ core

.PHONY: all clean
