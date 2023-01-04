# Makefile
SOURCE = \
 lib_cmdline_input__test.c lib_cmdline_input.c \
 lib_file__test.c lib_file.c \
 snippet_ifi__test.c snippet_ifi.c \
 lib_if-name-index__test.c snippet_ifi-name-index.c \
 lib_inet__test.c lib_inet.c \
 snippet_mcast__test.c snippet_mcast.c \
 lib_pthread__test.c lib_pthread.c \
 lib_read_write__test.c lib_read_write.c \
 snippet_route__test.c snippet_route.c \
 snippet_rtt__test.c snippet_rtt.c \
 snippet_socket__test.c snippet_socket.c \
 lib_socket__test.c lib_socket.c \
 lib_unix__test.c lib_unix.c \
 lib_error.c

OBJECTS = $(SOURCE:.c=.o)


.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: \
	lib_cmdline_input__test.elf \
	lib_file__test.elf \
	snippet_ifi__test.elf \
	lib_if-name-index__test.elf \
	lib_inet__test.elf \
	snippet_mcast__test.elf \
	lib_pthread__test.elf \
	lib_read_write__test.elf \
	snippet_route__test.elf \
	snippet_rtt__test.elf \
	snippet_socket__test.elf \
	lib_socket__test.elf \
	lib_unix__test.elf


lib_cmdline_input__test.elf: lib_cmdline_input__test.o lib_cmdline_input.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_file__test.elf: lib_file__test.o lib_file.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_if-name-index__test.elf: lib_if-name-index__test.o lib_if-name-index.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_inet__test.elf: lib_inet__test.o lib_inet.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_pthread__test.elf: lib_pthread__test.o lib_pthread.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^ -lm -pthread

lib_read_write__test.elf: lib_read_write__test.o lib_read_write.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_socket__test.elf: lib_socket__test.o lib_socket.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

lib_unix__test.elf: lib_unix__test.o lib_unix.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^


## snippets, code building pieces based on the library calls, e.g. a
## worker implementation for echo client or the like

snippet_ifi__test.elf: snippet_ifi__test.o snippet_ifi.o lib_unix.o lib_socket.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

snippet_mcast__test.elf: snippet_mcast__test.o snippet_mcast.o snippet_socket.o lib_socket.o lib_file.o lib_unix.o lib_read_write.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

snippet_route__test.elf: snippet_route__test.o snippet_route.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

snippet_rtt__test.elf: snippet_rtt__test.o snippet_rtt.o lib_unix.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^

snippet_socket__test.elf: snippet_socket__test.o snippet_socket.o lib_socket.o lib_file.o lib_unix.o lib_read_write.o lib_error.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	$(RM) *.o *.elf *~ core

.PHONY: all clean
