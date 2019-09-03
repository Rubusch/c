# Makefile
#
#

SOURCE = pointer_test.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic -std=c99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c ${SOURCE} $<

pointer.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: pointer.exe

clean:
	${RM} pointer.exe *.o *~ core
