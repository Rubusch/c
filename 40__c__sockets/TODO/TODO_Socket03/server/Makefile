# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = server.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Server: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Server

clean: 
	${RM} $(OBJECTS) Server *~ core


