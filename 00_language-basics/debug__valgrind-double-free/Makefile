# Makefile
#
#

SOURCE = Valgrind_double_free.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

Valgrind_double_free.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: Valgrind_double_free.exe

clean:
	${RM} Valgrind_double_free.exe *~ *.o core


