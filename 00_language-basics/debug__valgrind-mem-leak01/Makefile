# Makefile
#
#

SOURCE = Valgrind_mem_leak.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

Valgrind_mem_leak.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: Valgrind_mem_leak.exe

clean:
	${RM} Valgrind_mem_leak.exe *~ *.o core


