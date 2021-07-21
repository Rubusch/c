# Makefile
#
#

SOURCE = bintree.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

bintree.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: bintree.exe

.PHONY: clean

clean: 
	${RM} bintree.exe *~ *.o core


