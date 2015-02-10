# Makefile
#
# Single Linked Queue

SOURCE = main.c queue_item.c
HEADERS = queue_item.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<


all:	Queue.exe

Queue.exe: 	${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm


clean:
	${RM} ${OBJECTS} Queue.exe *~ core


