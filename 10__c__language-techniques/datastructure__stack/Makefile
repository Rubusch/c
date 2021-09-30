# Makefile
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
## @date: 2013-april-28

SOURCE = main.c stack.c
HEADERS = stack.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm

.c.o:
	${CC} -c ${CFLAGS} $<

Stack.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all:	Stack.exe

clean:
	${RM} -f ${OBJECTS} Stack.exe *~ *.log

