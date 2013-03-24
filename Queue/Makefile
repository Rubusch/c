# Makefile
#
# Single Linked Queue

SOURCE = main.c queue_item.c
HEADERS = queue_item.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm

.c.o:
	${CC} -c ${CFLAGS} $<

Queue: 	${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all:	Queue

clean:
	${RM} ${OBJECTS} *~ core


