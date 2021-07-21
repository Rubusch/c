# Makefile
#
#

SOURCE = allocation.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

allocation.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: allocation.exe

.PHONY: clean

clean: 
	${RM} allocation.exe *~ *.o core


