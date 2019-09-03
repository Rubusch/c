# Makefile
#
#

SOURCE = Valgrind_uninitialized_var.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

Valgrind_uninitialized_var.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: Valgrind_uninitialized_var.exe

clean:
	${RM} Valgrind_uninitialized_var.exe *~ *.o core


