# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = tcp_server.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

tcp_server: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: tcp_server

clean: 
	${RM} $(OBJECTS) tcp_server *~ core


