#	Makefile Dummy
#
#

SOURCE = test.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic -std=c99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

test.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

clean:
	${RM} test.exe ${OBJECTS} *~


