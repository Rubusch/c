# Makefile
SOURCE = lib_unix__test.c lib_unix.c lib_error.c
OBJECTS = $(SOURCE:.c=.o)


.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall #-Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


lib_unix__test.exe: lib_unix__test.o lib_unix.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

# TODO further lib_ targets

all: lib_unix__test.exe

clean:
	$(RM) $(OBJECTS) *.exe *~ core
