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
	${CC} -c ${CFLAGS} ${SOURCE} $<

pointer.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: pointer.exe

clean:
	${RM} pointer.exe *.o *~ core
