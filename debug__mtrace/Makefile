#	Makefile Dummy
#
#

SOURCE = test.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -pedantic
RM = rm

.c.o:
	${CC} -c ${CFLAGS} $<

test.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

clean:
	${RM} test.exe ${OBJECTS} *~


