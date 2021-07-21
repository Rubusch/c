# Makefile
#
#

SOURCE = avltree.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

avltree.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: avltree.exe

.PHONY: clean

clean: 
	${RM} avltree.exe *~ *.o core


