# Makefile
#
#

SOURCE = string.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

string.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: string.exe

.PHONY: clean

clean: 
	${RM} string.exe *~ *.o core


