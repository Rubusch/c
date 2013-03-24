# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = server.c
OBJECTS = ${SOURCE:.c=.o}
HEADERS = ../unixsocket.h
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

server: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: server

clean: 
	${RM} $(OBJECTS) server *~ core


