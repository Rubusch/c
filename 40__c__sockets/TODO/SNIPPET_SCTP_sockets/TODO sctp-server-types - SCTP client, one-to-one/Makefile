# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = ../lib_socket/error.c ../lib_socket/hstrerror.c ../lib_socket/if_name_index.c ../lib_socket/lib_socket.c ../lib_socket/mcast.c ../lib_socket/pselect.c ../lib_socket/snprintf.c ../lib_socket/wrap_lib.c ../lib_socket/wrap_sock.c ../lib_socket/wrap_stdio.c ../lib_socket/wrap_unix.c ../lib_socket/gai_strerror.c client.c
OBJECTS = ${SOURCE:.c=.o}
HEADERS = ../lib_socket/config.h ../lib_socket/lib_socket.h ../lib_socket/addrinfo.h 

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

## without header:
#.c.o:
#	${CC} -c ${CFLAGS} $<

client.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -pthread -sctp

all: client.exe

clean: 
	${RM} $(OBJECTS) client.exe *~ core


