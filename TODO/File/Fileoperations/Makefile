# Makefile
#
#

SOURCE = main.c fo.c
HEADERS = fo.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Fileoperations: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Fileoperations

clean:
	${RM} Fileoperations.exe ${OBJECTS} *~ *.txt core

