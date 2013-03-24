# Makefile
#
#

SOURCE = fsetposition.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o: 
	${CC} -c ${CFLAGS} $<

fsetposition: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: fsetposition

clean:
	${RM} fileposition.exe *~ *.o core
