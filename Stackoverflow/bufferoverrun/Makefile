## Makefile
##
##

SOURCE = overrun.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

overrun: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: overrun

clean:
	${RM} ${OBJECTS} overrun *~ core
