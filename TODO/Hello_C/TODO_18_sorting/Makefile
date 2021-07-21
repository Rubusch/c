# Makefile
#
#

SOURCE = sorting.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

sorting.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: sorting.exe

.PHONY: clean

clean: 
	${RM} sorting.exe *~ *.o core


