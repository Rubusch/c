# Makefile
SOURCE = \
 lib_cmdline_input__test.c lib_cmdline_input.c \
 lib_file__test.c lib_file.c \
 lib_ifi__test.c lib_ifi.c \
 lib_if-name-index__test.c lib_ifi-name-index.c \
 lib_inet__test.c lib_inet.c \
 lib_mcast__test.c lib_mcast.c \
 lib_pthread__test.c lib_pthread.c \
 lib_read_write__test.c lib_read_write.c \
 lib_route__test.c lib_route.c \
 lib_rtt__test.c lib_rtt.c \
 lib_socket_demo__test.c lib_socket_demo.c \
 lib_sock__test.c lib_sock.c \
 lib_streams__test.c lib_streams.c \
 lib_unix__test.c lib_unix.c \
 lib_error.c

OBJECTS = $(SOURCE:.c=.o)


.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall #-Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: \
	lib_cmdline_input__test.exe \
	lib_file__test.exe \
	lib_ifi__test.exe \
	lib_if-name-index__test.exe \
	lib_inet__test.exe \
	lib_mcast__test.exe \
	lib_pthread__test.exe \
	lib_read_write__test.exe \
	lib_route__test.exe \
	lib_rtt__test.exe \
	lib_socket_demo__test.exe \
	lib_sock__test.exe \
	lib_streams__test.exe \
	lib_unix__test.exe


lib_cmdline_input__test.exe: lib_cmdline_input__test.o lib_cmdline_input.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_file__test.exe: lib_file__test.o lib_file.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

## TODO rename, these are code snippets based on the above library snippets...     
lib_ifi__test.exe: lib_ifi__test.o lib_ifi.o lib_unix.o lib_sock.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_if-name-index__test.exe: lib_if-name-index__test.o lib_if-name-index.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_inet__test.exe: lib_inet__test.o lib_inet.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

## TODO rename, these are code snippets based on the above library snippets...     
lib_mcast__test.exe: lib_mcast__test.o lib_mcast.o lib_socket_demo.o lib_sock.o lib_streams.o lib_unix.o lib_read_write.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_pthread__test.exe: lib_pthread__test.o lib_pthread.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^ -lm -pthread

lib_read_write__test.exe: lib_read_write__test.o lib_read_write.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_route__test.exe: lib_route__test.o lib_route.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

## TODO rename, these are code snippets based on the above library snippets...     
lib_rtt__test.exe: lib_rtt__test.o lib_rtt.o lib_unix.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

## TODO rename, these are code snippets based on the above library snippets...     
lib_socket_demo__test.exe: lib_socket_demo__test.o lib_socket_demo.o lib_sock.o lib_streams.o lib_unix.o lib_read_write.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_sock__test.exe: lib_sock__test.o lib_sock.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_streams__test.exe: lib_streams__test.o lib_streams.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_unix__test.exe: lib_unix__test.o lib_unix.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	$(RM) *.o *.exe *~ core

.PHONY: all clean
