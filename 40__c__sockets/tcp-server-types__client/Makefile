# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


#SOURCE = ../lib_socket/error.c ../lib_socket/hstrerror.c ../lib_socket/if_name_index.c ../lib_socket/lib_socket.c ../lib_socket/mcast.c ../lib_socket/pselect.c ../lib_socket/snprintf.c ../lib_socket/wrap_lib.c ../lib_socket/wrap_sock.c ../lib_socket/wrap_stdio.c ../lib_socket/wrap_unix.c ../lib_socket/gai_strerror.c client.c

SOURCE = client.c
OBJECTS = $(SOURCE:.c=.o)
#HEADERS = ../lib_socket/config.h ../lib_socket/lib_socket.h ../lib_socket/addrinfo.h 
TARGET = $(SOURCE:.c=.exe)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

## without header:
#.c.o:
#	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -pthread

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~ core


