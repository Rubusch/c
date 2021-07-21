# Makefile
#
#

SOURCE = 2-dim-array.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

2-dim-array.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: 2-dim-array.exe

.PHONY: clean

clean: 
	${RM} 2-dim-array.exe *~ *.o core


