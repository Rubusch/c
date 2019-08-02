# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = client.c
OBJECTS = ${SOURCE:.c=.o}
HEADERS = ../unixsocket.h
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

client: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: client

clean: 
	${RM} $(OBJECTS) client *~ core


