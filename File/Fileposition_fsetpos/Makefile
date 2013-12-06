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

fsetposition.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: fsetposition.exe

clean:
	${RM} fsetposition.exe *~ *.o core
