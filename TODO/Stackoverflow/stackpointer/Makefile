## Makefile
##
##

SOURCE = stackpointer.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

stackpointer: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: stackpointer

clean:
	${RM} ${OBJECTS} stackpointer *~ core
