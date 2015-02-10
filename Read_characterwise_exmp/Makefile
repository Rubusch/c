# Makefile
#
#

SOURCE = test.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O1 -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

test: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: test

clean:
	${RM} test.exe ${OBJECTS} *~ core

